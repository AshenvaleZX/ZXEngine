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
			mCurTime = 0.0f;
		}
	}

	void Animation::Stop()
	{
		mIsPlaying = false;
	}

	bool Animation::IsPlaying() const
	{
		return mIsPlaying;
	}

	void Animation::Update(BoneNode* pBoneNode, const vector<Mesh*>& pMeshes)
	{
		if (!mIsPlaying)
			return;

		mCurTime += Time::deltaTime * mSpeed;

		if (mCurTime >= mDuration)
		{
			if (mIsLoop)
			{
				mCurTime -= mDuration;
			}
			else
			{
				mCurTime = 0.0f;
				Stop();
			}
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
			Vector3 scale = pNodeAnimation->GetScale(mCurTime);
			Matrix4 scaleMatrix = Math::ScaleMatrix(scale);

			Vector3 translation = pNodeAnimation->GetPosition(mCurTime);
			Matrix4 translationMatrix = Math::TranslationMatrix(translation);

			Quaternion rotation = pNodeAnimation->GetRotation(mCurTime);
			Matrix4 rotationMatrix = rotation.ToMatrix();

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		Matrix4 globalTransform = parentTransform * nodeTransform;

		for (auto pMesh : pMeshes)
		{
			if (pMesh->mBoneNameToIndexMap.find(nodeName) != pMesh->mBoneNameToIndexMap.end())
			{
				uint32_t index = pMesh->mBoneNameToIndexMap[nodeName];
				pMesh->mBones[index] = pMesh->mRootBoneToWorld * globalTransform * pMesh->mBones[index].offset;
			}
		}

		for (auto pChild : pNode->children)
		{
			GetNodeTransform(pChild, globalTransform, pMeshes);
		}
	}
}