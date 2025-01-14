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

	void Animation::Update(float deltaTime)
	{
		if (!mIsPlaying)
			return;

		mCurTick += deltaTime * mSpeed * mTicksPerSecond;

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

	void Animation::ForceUpdate(float tick)
	{
		mCurTick = tick;
		UpdateNodeAnimations();
	}

	void Animation::UpdateMeshes(const BoneNode* pBoneNode, const vector<shared_ptr<Mesh>>& pMeshes)
	{
		UpdateBoneTransforms(pBoneNode, Matrix4::Identity);

		for (auto& pMesh : pMeshes)
		{
			for (auto& iter : mBoneTransforms)
			{
				if (pMesh->mBoneNameToIndexMap.find(iter.first) != pMesh->mBoneNameToIndexMap.end())
				{
					uint32_t index = pMesh->mBoneNameToIndexMap[iter.first];
					// 此处的矩阵是给Shader用的，需要转置为列主序
					pMesh->mBonesFinalTransform[index] = Math::Transpose(iter.second * pMesh->mBonesOffset[index] * pMesh->mRootTrans);
				}
			}
		}
	}

	void Animation::UpdateFinalTransforms(const BoneNode* pBoneNode,
		const vector<vector<Matrix4>>& bonesOffsets,
		const vector<unordered_map<string, uint32_t>>& boneNameToIndexMaps,
		vector<vector<Matrix4>>& bonesFinalTransforms)
	{
		UpdateBoneTransforms(pBoneNode, Matrix4::Identity);

		for (size_t i = 0; i < bonesOffsets.size(); i++)
		{
			for (auto& iter : mBoneTransforms)
			{
				if (boneNameToIndexMaps[i].find(iter.first) != boneNameToIndexMaps[i].end())
				{
					uint32_t index = boneNameToIndexMaps[i].at(iter.first);
					// 此处的矩阵是给Shader用的，需要转置为列主序
					bonesFinalTransforms[i][index] = Math::Transpose(iter.second * bonesOffsets[i][index]);
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

	bool Animation::GetCurFrameByNode(const string& nodeName, KeyFrame& keyFrame)
	{
		auto pNodeAnim = GetNodeAnimation(nodeName);

		if (pNodeAnim)
			keyFrame = pNodeAnim->mCurFrame;

		return pNodeAnim != nullptr;
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

			Matrix4 rotationMatrix = pNodeAnimation->mCurFrame.mRotation.ToMatrix4();

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