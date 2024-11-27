#include "ParticleSystem.h"
#include "Transform.h"
#include "../ParticleSystem/ParticleSystemManager.h"

namespace ZXEngine
{
	ComponentType ParticleSystem::GetType()
	{
		return ComponentType::ParticleSystem;
	}

	ParticleSystem::ParticleSystem()
	{
		ParticleSystemManager::GetInstance()->AddParticleSystem(this);
	}

	ParticleSystem::~ParticleSystem()
	{
		ParticleSystemManager::GetInstance()->RemoveParticleSystem(this);
	}

	ComponentType ParticleSystem::GetInsType()
	{
		return ComponentType::ParticleSystem;
	}

	void ParticleSystem::Update()
	{
		// 更新当前位置和移动方向
		mState.mCurPos = GetTransform()->GetPosition();
		mState.mMoveDir = (mState.mCurPos - mState.mLastPos).GetNormalized();
		mState.mLastPos = mState.mCurPos;

		mEmitter.Emit(mState, mParticles);
	}

	void ParticleSystem::Render(Camera* camera)
	{
		mRenderer.Render(camera, mParticles);
	}

	void ParticleSystem::SetTexture(const string& path)
	{
		mRenderer.SetTexture(path);
	}

	void ParticleSystem::InternalGeneration()
	{
		mParticles.resize(mState.mMaxParticleNum);

		mRenderer.Init(mState);
	}
}