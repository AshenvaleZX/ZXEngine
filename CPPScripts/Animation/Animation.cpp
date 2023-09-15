#include "Animation.h"
#include "NodeAnimation.h"
#include "../Time.h"
#include "../ZMesh.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	Animation::~Animation()
	{
		for (auto& iter : mNodeAnimations)
		{
			delete iter.second;
		}
	}
	
	void Animation::Play(bool isReplay)
	{
		mIsPlaying = true;
		
		if (isReplay)
		{
			mCurTick = 0.0f;
			Reset();
		}
	}

	void Animation::Stop()
	{
		mIsPlaying = false;
	}

	void Animation::Reset()
	{
		for (auto& pNodeAnim : mNodeAnimations)
		{
			pNodeAnim.second->Reset();
		}
	}

	bool Animation::IsPlaying() const
	{
		return mIsPlaying;
	}

	void Animation::Update()
	{
		if (!mIsPlaying)
			return;

		mCurTick += Time::deltaTime * mSpeed * mTicksPerSecond;

		if (mCurTick >= mFullTick)
		{
			if (mIsLoop)
			{
				mCurTick -= mFullTick;
			}
			else
			{
				mCurTick = 0.0f;
				Stop();
			}
			Reset();
		}

		UpdateNodeAnimations();
	}

	void Animation::UpdateMeshes(const BoneNode* pBoneNode, const vector<Mesh*>& pMeshes)
	{
		UpdateBoneTransforms(pBoneNode, Matrix4());

		for (auto& pMesh : pMeshes)
		{
			for (auto& iter : mBoneTransforms)
			{
				if (pMesh->mBoneNameToIndexMap.find(iter.first) != pMesh->mBoneNameToIndexMap.end())
				{
					uint32_t index = pMesh->mBoneNameToIndexMap[iter.first];
					// 此处的矩阵是给Shader用的，需要转置为列主序
					pMesh->mBonesFinalTransform[index] = Math::Transpose(iter.second * pMesh->mBonesOffset[index]);
				}
			}
		}
	}

	void Animation::AddNodeAnimation(NodeAnimation* nodeAnimation)
	{
		if (mNodeAnimations.find(nodeAnimation->mName) == mNodeAnimations.end())
		{
			mNodeAnimations[nodeAnimation->mName] = nodeAnimation;
		}
		else
		{
			Debug::LogWarning("Animation try to add an existing node animation: %s", nodeAnimation->mName);
		}
	}

	void Animation::UpdateNodeAnimations()
	{
		for (auto& iter : mNodeAnimations)
		{
			iter.second->UpdateCurFrame(mCurTick);
		}
	}

	void Animation::UpdateBoneTransforms(const BoneNode* pBoneNode, const Matrix4& parentTransform)
	{
		string nodeName = pBoneNode->name;

		Matrix4 nodeTransform(pBoneNode->transform);

		NodeAnimation* pNodeAnimation = GetNodeAnimation(nodeName);

		if (pNodeAnimation)
		{
			Matrix4 scaleMatrix = Math::ScaleMatrix(pNodeAnimation->mCurFrame.mScale);

			Matrix4 translationMatrix = Math::TranslationMatrix(pNodeAnimation->mCurFrame.mPosition);

			Matrix4 rotationMatrix = pNodeAnimation->mCurFrame.mRotation.ToMatrix();

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		Matrix4 globalTransform = parentTransform * nodeTransform;

		mBoneTransforms[nodeName] = globalTransform;

		for (auto pChild : pBoneNode->children)
		{
			UpdateBoneTransforms(pChild, globalTransform);
		}
	}

	NodeAnimation* Animation::GetNodeAnimation(const string& nodeName)
	{
		if (mNodeAnimations.find(nodeName) != mNodeAnimations.end())
		{
			return mNodeAnimations[nodeName];
		}
		else
		{
			return nullptr;
		}
	}
}