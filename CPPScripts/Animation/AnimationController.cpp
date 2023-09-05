#include "AnimationController.h"
#include "Animation.h"
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

	void AnimationController::Update(BoneNode* pBoneNode, const vector<Mesh*>& pMeshes)
	{
		if (mCurAnimation)
		{
			mCurAnimation->Update(pBoneNode, pMeshes);
		}
	}

	void AnimationController::AddAnimation(Animation* anim)
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

	void AnimationController::PlayAnimation(const string& name)
	{
		if (mAnimations.find(name) != mAnimations.end())
		{
			mCurAnimation = mAnimations[name];
		}
		else
		{
			Debug::LogWarning("AnimationController try to play an non-existing animation: %s", name);
		}
	}
}