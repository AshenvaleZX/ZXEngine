#include "Animator.h"
#include "MeshRenderer.h"
#include "../Animation/AnimationController.h"

// 动画系统异步更新的开关，暂时先关闭
// 能正常用不会有异常，但是性能优化效果并不理想
// #define ENABLE_ASYNC_ANIMATION

namespace ZXEngine
{
	ComponentType Animator::GetType()
	{
		return ComponentType::Animator;
	}

	vector<Animator*> Animator::mAnimators;

	void Animator::Update()
	{
#ifdef ENABLE_ASYNC_ANIMATION
		AsyncUpdate();
#else
		SyncUpdate();
#endif
	}

	void Animator::SyncUpdate()
	{
		for (auto pAnimator : mAnimators)
			pAnimator->UpdateMeshes(false);
	}

	void Animator::AsyncUpdate()
	{
		for (auto pAnimator : mAnimators)
			pAnimator->UpdateMeshes(true);

		for (auto pAnimator : mAnimators)
			pAnimator->AccomplishUpdate();
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

	void Animator::UpdateMeshes(bool async)
	{
		mAnimationController->Update(mRootBoneNode, mMeshRenderer->mMeshes, async);
	}

	void Animator::AccomplishUpdate()
	{
		mAnimationController->AccomplishUpdate(mMeshRenderer->mMeshes);
	}
}