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
		Vector3 pos = GetTransform()->GetPosition();
		mState.mMoveDir = (pos - mState.mCurPos).GetNormalized();
		mState.mCurPos = std::move(pos);
		mState.mCurRot = GetTransform()->GetRotation();

		mEmitter.Update(mState, mParticles);
		mEvolver.Update(mState, mParticles);
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