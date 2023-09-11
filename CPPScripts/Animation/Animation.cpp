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

	void Animation::Update(const BoneNode* pBoneNode, const vector<Mesh*>& pMeshes)
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

		GetNodeTransform(pBoneNode, Matrix4(), pMeshes);
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

	void Animation::GetNodeTransform(const BoneNode* pNode, const Matrix4& parentTransform, const vector<Mesh*>& pMeshes)
	{
		string nodeName = pNode->name;

		Matrix4 nodeTransform(pNode->transform);

		NodeAnimation* pNodeAnimation = GetNodeAnimation(nodeName);

		if (pNodeAnimation)
		{
			Vector3 scale = pNodeAnimation->GetScale(mCurTick);
			Matrix4 scaleMatrix = Math::ScaleMatrix(scale);

			Vector3 translation = pNodeAnimation->GetPosition(mCurTick);
			Matrix4 translationMatrix = Math::TranslationMatrix(translation);

			Quaternion rotation = pNodeAnimation->GetRotation(mCurTick);
			Matrix4 rotationMatrix = rotation.ToMatrix();

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		Matrix4 globalTransform = parentTransform * nodeTransform;

		for (auto pMesh : pMeshes)
		{
			if (pMesh->mBoneNameToIndexMap.find(nodeName) != pMesh->mBoneNameToIndexMap.end())
			{
				uint32_t index = pMesh->mBoneNameToIndexMap[nodeName];
				// 此处的矩阵是给Shader用的，需要转置为列主序
				pMesh->mBonesFinalTransform[index] = Math::Transpose(globalTransform * pMesh->mBonesOffset[index]);
			}
		}

		for (auto pChild : pNode->children)
		{
			GetNodeTransform(pChild, globalTransform, pMeshes);
		}
	}
}