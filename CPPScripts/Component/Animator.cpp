#include "Animator.h"
#include "MeshRenderer.h"
#include "../Animation/AnimationController.h"

namespace ZXEngine
{
	ComponentType Animator::GetType()
	{
		return ComponentType::Animator;
	}

	vector<Animator*> Animator::mAnimators;

	void Animator::Update()
	{
		for (auto pAnimator : mAnimators)
			pAnimator->UpdateMeshes();
	}

	ComponentType Animator::GetInsType()
	{
		return ComponentType::Animator;
	}

	Animator::Animator()
	{
		mAnimators.push_back(this);
	}

	Animator::~Animator()
	{
		if (mRootBoneNode)
			delete mRootBoneNode;
		if (mAnimationController)
			delete mAnimationController;

		auto iter = std::find(mAnimators.begin(), mAnimators.end(), this);
		mAnimators.erase(iter);
	}

	void Animator::Play(const string& name)
	{
		mAnimationController->Play(name);
	}

	void Animator::Switch(const string& name, float time)
	{
		mAnimationController->Switch(name, time);
	}

	void Animator::UpdateMeshes()
	{
		mAnimationController->Update(mRootBoneNode, mMeshRenderer->mMeshes);
	}
}