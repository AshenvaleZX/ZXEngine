#include "Animator.h"
#include "MeshRenderer.h"
#include "../ZMesh.h"
#include "../RenderAPI.h"
#include "../Resources.h"
#include "../Animation/AnimationController.h"

// 动画系统异步更新的开关，暂时先关闭
// 能正常用不会有异常，但是性能优化效果并不理想
// #define ENABLE_ASYNC_ANIMATION

namespace ZXEngine
{
	ComputeShaderReference* Animator::mComputeShader = nullptr;

	ComponentType Animator::GetType()
	{
		return ComponentType::Animator;
	}

	vector<Animator*> Animator::mAnimators;

	void Animator::Update()
	{
#ifdef ZX_COMPUTE_SHADER_SUPPORT
		if (mComputeShader == nullptr)
			mComputeShader = RenderAPI::GetInstance()->LoadAndSetUpComputeShader(Resources::GetAssetFullPath("Shaders/Animation.zxcompute", true));
#endif

#ifdef ENABLE_ASYNC_ANIMATION
		AsyncUpdate();
#else
		SyncUpdate();
#endif
	}

	void Animator::SyncUpdate()
	{
		for (auto pAnimator : mAnimators)
		{
			pAnimator->UpdateMeshes(false);
#ifdef ZX_COMPUTE_SHADER_SUPPORT
			pAnimator->UpdateVertices();
#endif
		}
	}

	void Animator::AsyncUpdate()
	{
		for (auto pAnimator : mAnimators)
			pAnimator->UpdateMeshes(true);

		for (auto pAnimator : mAnimators)
			pAnimator->AccomplishUpdate();

#ifdef ZX_COMPUTE_SHADER_SUPPORT
		for (auto pAnimator : mAnimators)
			pAnimator->UpdateVertices();
#endif
	}

	ComponentType Animator::GetInsType()
	{
		return ComponentType::Animator;
	}

	Animator::Animator()
	{
		mAnimators.push_back(this);
		mCommand = RenderAPI::GetInstance()->AllocateDrawCommand(CommandType::Compute, 0);
	}

	Animator::~Animator()
	{
		if (mRootBoneNode)
			delete mRootBoneNode;
		if (mAnimationController)
			delete mAnimationController;

		RenderAPI::GetInstance()->FreeDrawCommand(mCommand);

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

	void Animator::UpdateVertices()
	{
		auto renderAPI = RenderAPI::GetInstance();

		for (auto& pMesh : mMeshRenderer->mMeshes)
		{
			renderAPI->UpdateShaderStorageBuffer(pMesh->mBoneTransformSSBO, pMesh->mBonesFinalTransform.data(), pMesh->mBonesFinalTransform.size() * sizeof(Matrix4));

			renderAPI->BindVertexBuffer(pMesh->VAO, 0);
			renderAPI->BindShaderStorageBuffer(pMesh->mVertexSSBO, 1);
			renderAPI->BindShaderStorageBuffer(pMesh->mBoneTransformSSBO, 2);

			renderAPI->Dispatch(mCommand, mComputeShader->ID, static_cast<uint32_t>(pMesh->mVertices.size() + 63) / 64, 1, 1);
		}
	}
}