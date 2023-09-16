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
		mAnimationController->Play(name);
	}

	void Animator::Switch(const string& name, float time)
	{
		mAnimationController->Switch(name, time);
	}

	void Animator::Update(const vector<Mesh*>& pMeshes)
	{
		mAnimationController->Update(mRootBoneNode, pMeshes);
	}
}