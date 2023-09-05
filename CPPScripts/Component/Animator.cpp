#include "Animator.h"
#include "../Animation/AnimationController.h"

namespace ZXEngine
{
	ComponentType Animator::GetType()
	{
		return ComponentType::Animator;
	}

	ComponentType Animator::GetInsType()
	{
		return ComponentType::Animator;
	}

	Animator::~Animator()
	{
		if (mRootBoneNode)
			delete mRootBoneNode;
		if (mAnimationController)
			delete mAnimationController;
	}

	void Animator::Play(const string& name)
	{
		mAnimationController->PlayAnimation(name);
	}
}