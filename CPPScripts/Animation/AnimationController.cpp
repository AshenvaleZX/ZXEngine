#include "AnimationController.h"
#include "Animation.h"
#include "../Time.h"
#include "../ZMesh.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	void AnimationUpdateJob::Execute()
	{
		mAnimation->Update(mDeltaTime);
		mAnimation->UpdateFinalTransforms(mBoneNode, mBonesOffsets, mBoneNameToIndexMaps, mBonesFinalTransforms);
	}

	void AnimationBlendUpdateJob::Execute()
	{
		mCurAnimation->ForceUpdate(mCurAnimTick);
		mTargetAnimation->ForceUpdate(mTargetAnimTick);

		UpdateBlendTransforms(mBoneNode, Matrix4());
	}

	void AnimationBlendUpdateJob::UpdateBlendTransforms(const BoneNode* pBoneNode, const Matrix4& parentTransform)
	{
		string nodeName = pBoneNode->name;

		Matrix4 nodeTransform(pBoneNode->transform);

		KeyFrame curKeyFrame;
		KeyFrame targetKeyFrame;

		bool getCurFrame = mCurAnimation->GetCurFrameByNode(nodeName, curKeyFrame);
		bool getTargetFrame = mTargetAnimation->GetCurFrameByNode(nodeName, targetKeyFrame);

		if (getCurFrame && getTargetFrame)
		{
			Vector3 scale = Math::Lerp(curKeyFrame.mScale, targetKeyFrame.mScale, mBlendFactor);
			Matrix4 scaleMatrix = Math::ScaleMatrix(scale);

			Vector3 position = Math::Lerp(curKeyFrame.mPosition, targetKeyFrame.mPosition, mBlendFactor);
			Matrix4 translationMatrix = Math::TranslationMatrix(position);

			Quaternion rotation = Math::Slerp(curKeyFrame.mRotation, targetKeyFrame.mRotation, mBlendFactor);
			Matrix4 rotationMatrix = rotation.ToMatrix();

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}
		else if (getCurFrame != getTargetFrame)
		{
			Debug::LogWarning("Blend animation do not match on node: %s", nodeName);
		}

		Matrix4 globalTransform = parentTransform * nodeTransform;

		for (size_t i = 0; i < mBonesOffsets.size(); i++)
		{
			if (mBoneNameToIndexMaps[i].find(nodeName) != mBoneNameToIndexMaps[i].end())
			{
				uint32_t index = mBoneNameToIndexMaps[i][nodeName];
				// 此处的矩阵是给Shader用的，需要转置为列主序
				mBonesFinalTransforms[i][index] = Math::Transpose(globalTransform * mBonesOffsets[i][index]);
			}
		}

		for (auto& iter : pBoneNode->children)
		{
			UpdateBlendTransforms(iter, globalTransform);
		}
	}

	AnimationController::~AnimationController()
	{
		for (auto& iter : mAnimations)
		{
			delete iter.second;
		}
	}

	void AnimationController::Update(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes, bool async)
	{
		if (mIsBlending)
		{
			mBlendTime += Time::deltaTime;

			if (mBlendTime > mBlendDuration)
			{
				mIsBlending = false;
				mCurAnimation = mTargetAnimation;
				mTargetAnimation = nullptr;
				mBlendTime = 0.0f;
				mBlendDuration = 0.0f;
				mCurBlendAnimTime = 0.0f;
				mBlendFactor = 0.0f;
			}
			else
			{
				mBlendFactor = mBlendTime / mBlendDuration;

				// 混合动画时长
				float blendAnimDuration = Math::Lerp(mCurAnimation->mDuration, mTargetAnimation->mDuration, mBlendFactor);

				// 混合动画播放进度
				float curAnimProgressDelta = Time::deltaTime / mCurAnimation->mDuration;
				float targetAnimProgressDelta = Time::deltaTime / mTargetAnimation->mDuration;
				float blendAnimProgressDelta = Math::Lerp(curAnimProgressDelta, targetAnimProgressDelta, mBlendFactor);
				
				mCurBlendAnimTime += blendAnimProgressDelta * blendAnimDuration;

				if (mCurBlendAnimTime > blendAnimDuration)
				{
					mCurAnimation->Reset();
					mTargetAnimation->Reset();
					mCurBlendAnimTime -= blendAnimDuration;
				}
				float blendAnimProgress = mCurBlendAnimTime / blendAnimDuration;

				// 混合动画的播放进度转换成tick
				float curAnimTicks = blendAnimProgress * mCurAnimation->mFullTick;
				float targetAnimTicks = blendAnimProgress * mTargetAnimation->mFullTick;

				if (async)
				{
					mBlendUpdateJob.mBoneNode = pBoneNode;
					mBlendUpdateJob.mCurAnimation = mCurAnimation;
					mBlendUpdateJob.mTargetAnimation = mTargetAnimation;
					mBlendUpdateJob.mBlendFactor = mBlendFactor;
					mBlendUpdateJob.mCurAnimTick = curAnimTicks;
					mBlendUpdateJob.mTargetAnimTick = targetAnimTicks;

					mBlendUpdateJob.mBonesOffsets.clear();
					mBlendUpdateJob.mBoneNameToIndexMaps.clear();
					mBlendUpdateJob.mBonesFinalTransforms.clear();
					for (auto& pMesh : pMeshes)
					{
						mBlendUpdateJob.mBonesOffsets.push_back(pMesh->mBonesOffset);
						mBlendUpdateJob.mBoneNameToIndexMaps.push_back(pMesh->mBoneNameToIndexMap);
						mBlendUpdateJob.mBonesFinalTransforms.push_back(pMesh->mBonesFinalTransform);
					}

					mBlendUpdateHandle = mBlendUpdateJob.Schedule();
				}
				else
				{
					mCurAnimation->ForceUpdate(curAnimTicks);
					mTargetAnimation->ForceUpdate(targetAnimTicks);

					UpdateBlendAnimation(pBoneNode, pMeshes, Matrix4());
				}
			}
		}
		else if (mCurAnimation)
		{
			if (async)
			{
				mUpdateJob.mBoneNode = pBoneNode;
				mUpdateJob.mAnimation = mCurAnimation;
				mUpdateJob.mDeltaTime = Time::deltaTime;

				mUpdateJob.mBonesOffsets.clear();
				mUpdateJob.mBoneNameToIndexMaps.clear();
				mUpdateJob.mBonesFinalTransforms.clear();
				for (auto& pMesh : pMeshes)
				{
					mUpdateJob.mBonesOffsets.push_back(pMesh->mBonesOffset);
					mUpdateJob.mBoneNameToIndexMaps.push_back(pMesh->mBoneNameToIndexMap);
					mUpdateJob.mBonesFinalTransforms.push_back(pMesh->mBonesFinalTransform);
				}

				mUpdateHandle = mUpdateJob.Schedule();
			}
			else
			{
				mCurAnimation->Update(Time::deltaTime);
				mCurAnimation->UpdateMeshes(pBoneNode, pMeshes);
			}
		}
	}

	void AnimationController::AccomplishUpdate(const vector<shared_ptr<Mesh>>& pMeshes)
	{
		if (mUpdateHandle.has_value())
		{
			mUpdateHandle->Accomplish();
			mUpdateHandle.reset();

			for (size_t i = 0; i < pMeshes.size(); i++)
			{
				pMeshes[i]->mBonesFinalTransform = std::move(mUpdateJob.mBonesFinalTransforms[i]);
			}
		}
		else if (mBlendUpdateHandle.has_value())
		{
			mBlendUpdateHandle->Accomplish();
			mBlendUpdateHandle.reset();

			for (size_t i = 0; i < pMeshes.size(); i++)
			{
				pMeshes[i]->mBonesFinalTransform = std::move(mBlendUpdateJob.mBonesFinalTransforms[i]);
			}
		}
	}

	void AnimationController::Add(Animation* anim)
	{
		if (mAnimations.find(anim->mName) == mAnimations.end())
		{
			mAnimations[anim->mName] = anim;
		}
		else
		{
			Debug::LogWarning("AnimationController try to add an existing animation: %s", anim->mName);
		}
	}

	void AnimationController::Play(const string& name)
	{
		if (mAnimations.find(name) != mAnimations.end())
		{
			// 这个接口会直接停止当前正在播放的动画
			if (mCurAnimation)
			{
				mCurAnimation->Stop();
			}

			mCurAnimation = mAnimations[name];
			mCurAnimation->Play();
		}
		else
		{
			Debug::LogWarning("AnimationController try to play an non-existing animation: %s", name);
		}
	}

	void AnimationController::Switch(const string& name, float time)
	{
		if (mAnimations.find(name) != mAnimations.end())
		{
			// 当前有动画正在播放才走混合切换逻辑
			if (mCurAnimation && mCurAnimation->IsPlaying())
			{
				// 如果当前动画和要切换的动画是同一个就不用切换了
				if (mCurAnimation->mName == name)
				{
					return;
				}

				mBlendTime = 0.0f;
				mBlendDuration = time;
				mIsBlending = true;
				// 混合刚开始的时候直接以当前动画的播放进度作为混合动画的播放进度
				mCurBlendAnimTime = mCurAnimation->mCurTick / mCurAnimation->mTicksPerSecond;

				mTargetAnimation = mAnimations[name];
				mTargetAnimation->Play();
			}
			// 当前没有正在播放的动画就直接播新动画
			else
			{
				mCurAnimation = mAnimations[name];
				mCurAnimation->Play();
			}
		}
		else
		{
			Debug::LogWarning("AnimationController try to switch to an non-existing animation: %s", name);
		}
	}

	void AnimationController::UpdateBlendAnimation(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes, const Matrix4& parentTransform)
	{
		string nodeName = pBoneNode->name;

		Matrix4 nodeTransform(pBoneNode->transform);

		KeyFrame curKeyFrame;
		KeyFrame targetKeyFrame;

		bool getCurFrame = mCurAnimation->GetCurFrameByNode(nodeName, curKeyFrame);
		bool getTargetFrame = mTargetAnimation->GetCurFrameByNode(nodeName, targetKeyFrame);

		if (getCurFrame && getTargetFrame)
		{
			Vector3 scale = Math::Lerp(curKeyFrame.mScale, targetKeyFrame.mScale, mBlendFactor);
			Matrix4 scaleMatrix = Math::ScaleMatrix(scale);

			Vector3 position = Math::Lerp(curKeyFrame.mPosition, targetKeyFrame.mPosition, mBlendFactor);
			Matrix4 translationMatrix = Math::TranslationMatrix(position);

			Quaternion rotation = Math::Slerp(curKeyFrame.mRotation, targetKeyFrame.mRotation, mBlendFactor);
			Matrix4 rotationMatrix = rotation.ToMatrix();

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}
		else if (getCurFrame != getTargetFrame)
		{
			Debug::LogWarning("Blend animation do not match on node: %s", nodeName);
		}

		Matrix4 globalTransform = parentTransform * nodeTransform;

		for (auto& pMesh : pMeshes)
		{
			if (pMesh->mBoneNameToIndexMap.find(nodeName) != pMesh->mBoneNameToIndexMap.end())
			{
				uint32_t index = pMesh->mBoneNameToIndexMap[nodeName];
				// 此处的矩阵是给Shader用的，需要转置为列主序
				pMesh->mBonesFinalTransform[index] = Math::Transpose(globalTransform * pMesh->mBonesOffset[index]);
			}
		}

		for (auto& iter : pBoneNode->children)
		{
			UpdateBlendAnimation(iter, pMeshes, globalTransform);
		}
	}
}