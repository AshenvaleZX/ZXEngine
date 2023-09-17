#include "AnimationController.h"
#include "Animation.h"
#include "../Time.h"
#include "../ZMesh.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	AnimationController::~AnimationController()
	{
		for (auto& iter : mAnimations)
		{
			delete iter.second;
		}
	}

	void AnimationController::Update(const BoneNode* pBoneNode, const vector<Mesh*>& pMeshes)
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

				// ��϶���ʱ��
				float blendAnimDuration = Math::Lerp(mCurAnimation->mDuration, mTargetAnimation->mDuration, mBlendFactor);

				// ��϶������Ž���
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

				// ��϶����Ĳ��Ž���ת����tick
				float curAnimTicks = blendAnimProgress * mCurAnimation->mFullTick;
				float targetAnimTicks = blendAnimProgress * mTargetAnimation->mFullTick;

				mCurAnimation->Update(curAnimTicks);
				mTargetAnimation->Update(targetAnimTicks);

				UpdateBlendAnimation(pBoneNode, pMeshes, Matrix4());
			}
		}
		else if (mCurAnimation)
		{
			mCurAnimation->Update();
			mCurAnimation->UpdateMeshes(pBoneNode, pMeshes);
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
			// ����ӿڻ�ֱ��ֹͣ��ǰ���ڲ��ŵĶ���
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
			// ��ǰ�ж������ڲ��Ų��߻���л��߼�
			if (mCurAnimation && mCurAnimation->IsPlaying())
			{
				// �����ǰ������Ҫ�л��Ķ�����ͬһ���Ͳ����л���
				if (mCurAnimation->mName == name)
				{
					return;
				}

				mBlendTime = 0.0f;
				mBlendDuration = time;
				mIsBlending = true;
				// ��ϸտ�ʼ��ʱ��ֱ���Ե�ǰ�����Ĳ��Ž�����Ϊ��϶����Ĳ��Ž���
				mCurBlendAnimTime = mCurAnimation->mCurTick / mCurAnimation->mTicksPerSecond;

				mTargetAnimation = mAnimations[name];
				mTargetAnimation->Play();
			}
			// ��ǰû�����ڲ��ŵĶ�����ֱ�Ӳ��¶���
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

	void AnimationController::UpdateBlendAnimation(const BoneNode* pBoneNode, const vector<Mesh*>& pMeshes, const Matrix4& parentTransform)
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

		for (auto pMesh : pMeshes)
		{
			if (pMesh->mBoneNameToIndexMap.find(nodeName) != pMesh->mBoneNameToIndexMap.end())
			{
				uint32_t index = pMesh->mBoneNameToIndexMap[nodeName];
				// �˴��ľ����Ǹ�Shader�õģ���Ҫת��Ϊ������
				pMesh->mBonesFinalTransform[index] = Math::Transpose(globalTransform * pMesh->mBonesOffset[index]);
			}
		}

		for (auto& iter : pBoneNode->children)
		{
			UpdateBlendAnimation(iter, pMeshes, globalTransform);
		}
	}
}