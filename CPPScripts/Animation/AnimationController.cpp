#include "AnimationController.h"
#include "Animation.h"

namespace ZXEngine
{
	AnimationController::~AnimationController()
	{
		for (auto& iter : mAnimations)
		{
			delete iter.second;
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

	void AnimationController::PlayAnimation(string name)
	{
		if (mAnimations.find(name) != mAnimations.end())
		{
			mCurAnimation = mAnimations[name];
			mCurAnimation->Play();
		}
	}
}