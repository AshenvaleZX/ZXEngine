#include "AnimationController.h"
#include "Animation.h"
#include "../Time.h"
#include "../ZMesh.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	void AnimationUpdateJob::Execute()
	{
		mSafeData.Dump(mData);

		mData.mAnimation->Update(mData.mDeltaTime);
		mData.mAnimation->UpdateFinalTransforms(mData.mBoneNode, mData.mBonesOffsets, mData.mBoneNameToIndexMaps, mData.mBonesFinalTransforms);

		mSafeData.Move(mData);
	}

	void AnimationBlendUpdateJob::Execute()
	{
		mSafeData.Dump(mData);

		mData.mCurAnimation->ForceUpdate(mData.mCurAnimTick);
		mData.mTargetAnimation->ForceUpdate(mData.mTargetAnimTick);

		UpdateBlendTransforms(mData.mBoneNode, Matrix4::Identity);

		mSafeData.Move(mData);
	}

	void AnimationBlendUpdateJob::UpdateBlendTransforms(const BoneNode* pBoneNode, const Matrix4& parentTransform)
	{
		string nodeName = pBoneNode->name;

		Matrix4 nodeTransform(pBoneNode->transform);

		KeyFrame curKeyFrame;
		KeyFrame targetKeyFrame;

		bool getCurFrame = mData.mCurAnimation->GetCurFrameByNode(nodeName, curKeyFrame);
		bool getTargetFrame = mData.mTargetAnimation->GetCurFrameByNode(nodeName, targetKeyFrame);

		if (getCurFrame && getTargetFrame)
		{
			Vector3 scale = Math::Lerp(curKeyFrame.mScale, targetKeyFrame.mScale, mData.mBlendFactor);
			Matrix4 scaleMatrix = Math::ScaleMatrix(scale);

			Vector3 position = Math::Lerp(curKeyFrame.mPosition, targetKeyFrame.mPosition, mData.mBlendFactor);
			Matrix4 translationMatrix = Math::TranslationMatrix(position);

			Quaternion rotation = Math::Slerp(curKeyFrame.mRotation, targetKeyFrame.mRotation, mData.mBlendFactor);
			Matrix4 rotationMatrix = rotation.ToMatrix();

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}
		else if (getCurFrame != getTargetFrame)
		{
			Debug::LogWarning("Blend animation do not match on node: %s", nodeName);
		}

		Matrix4 globalTransform = parentTransform * nodeTransform;

		for (size_t i = 0; i < mData.mBonesOffsets.size(); i++)
		{
			if (mData.mBoneNameToIndexMaps[i].find(nodeName) != mData.mBoneNameToIndexMaps[i].end())
			{
				uint32_t index = mData.mBoneNameToIndexMaps[i][nodeName];
				// 此处的矩阵是给Shader用的，需要转置为列主序
				mData.mBonesFinalTransforms[i][index] = Math::Transpose(globalTransform * mData.mBonesOffsets[i][index]);
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
					mBlendUpdateJob.mData.mBoneNode = pBoneNode;
					mBlendUpdateJob.mData.mCurAnimation = mCurAnimation;
					mBlendUpdateJob.mData.mTargetAnimation = mTargetAnimation;
					mBlendUpdateJob.mData.mBlendFactor = mBlendFactor;
					mBlendUpdateJob.mData.mCurAnimTick = curAnimTicks;
					mBlendUpdateJob.mData.mTargetAnimTick = targetAnimTicks;

					mBlendUpdateJob.mData.mBonesOffsets.clear();
					mBlendUpdateJob.mData.mBoneNameToIndexMaps.clear();
					mBlendUpdateJob.mData.mBonesFinalTransforms.clear();
					for (auto& pMesh : pMeshes)
					{
						mBlendUpdateJob.mData.mBonesOffsets.push_back(pMesh->mBonesOffset);
						mBlendUpdateJob.mData.mBoneNameToIndexMaps.push_back(pMesh->mBoneNameToIndexMap);
						mBlendUpdateJob.mData.mBonesFinalTransforms.push_back(pMesh->mBonesFinalTransform);
					}

					mBlendUpdateJob.mSafeData.Move(mBlendUpdateJob.mData);

					mBlendUpdateHandle = mBlendUpdateJob.Schedule();
				}
				else
				{
					mCurAnimation->ForceUpdate(curAnimTicks);
					mTargetAnimation->ForceUpdate(targetAnimTicks);

					UpdateBlendAnimation(pBoneNode, pMeshes, Matrix4::Identity);
				}
			}
		}
		else if (mCurAnimation)
		{
			if (async)
			{
				mUpdateJob.mData.mBoneNode = pBoneNode;
				mUpdateJob.mData.mAnimation = mCurAnimation;
				mUpdateJob.mData.mDeltaTime = Time::deltaTime;

				mUpdateJob.mData.mBonesOffsets.clear();
				mUpdateJob.mData.mBoneNameToIndexMaps.clear();
				mUpdateJob.mData.mBonesFinalTransforms.clear();
				for (auto& pMesh : pMeshes)
				{
					mUpdateJob.mData.mBonesOffsets.push_back(pMesh->mBonesOffset);
					mUpdateJob.mData.mBoneNameToIndexMaps.push_back(pMesh->mBoneNameToIndexMap);
					mUpdateJob.mData.mBonesFinalTransforms.push_back(pMesh->mBonesFinalTransform);
				}

				mUpdateJob.mSafeData.Move(mUpdateJob.mData);

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

			mUpdateJob.mSafeData.Dump(mUpdateJob.mData);

			for (size_t i = 0; i < pMeshes.size(); i++)
			{
				pMeshes[i]->mBonesFinalTransform = std::move(mUpdateJob.mData.mBonesFinalTransforms[i]);
			}
		}
		else if (mBlendUpdateHandle.has_value())
		{
			mBlendUpdateHandle->Accomplish();
			mBlendUpdateHandle.reset();

			mBlendUpdateJob.mSafeData.Dump(mBlendUpdateJob.mData);

			for (size_t i = 0; i < pMeshes.size(); i++)
			{
				pMeshes[i]->mBonesFinalTransform = std::move(mBlendUpdateJob.mData.mBonesFinalTransforms[i]);
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