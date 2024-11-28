#include "ParticleEmitter.h"
#include "../Time.h"

namespace ZXEngine
{
	void ParticleEmitter::Update(const ParticleSystemState& state, vector<Particle>& particles)
	{
		// ����������ʱ����
		float interval = 1.0f / state.mEmissionState.mRate;
		float curTime = Time::curTime;
		uint32_t genNum = 0;
		if (mLastGenTime == 0)
		{
			// ��һ֡�ȳ�ʼ������
			mLastGenTime = curTime;
		}
		else
		{
			// ��һ���������ӵ�ʱ��
			float delta = curTime - mLastGenTime;
			// ������ʱ������������������ô�����������ʱ�䣬������һ֡Ӧ������������
			if (delta > interval)
				genNum = static_cast<uint32_t>(delta / interval);
			// �����һ֡���������ӣ���ô��������ʱ��
			if (genNum > 0)
				mLastGenTime = curTime;
		}

		for (uint32_t i = 0; i < genNum; i++)
		{
			auto idx = GetUnusedParticleIndex(state, particles);
			RespawnParticle(state, particles[idx]);
		}
	}

	uint32_t ParticleEmitter::GetUnusedParticleIndex(const ParticleSystemState& state, vector<Particle>& particles)
	{
		// ����һ���ù��������
		for (uint32_t i = mLastUsedIndex; i < state.mMaxParticleNum; ++i)
		{
			if (particles[i].life <= 0.0f)
			{
				mLastUsedIndex = i;
				return i;
			}
		}
		// û�ҵ��ʹ�ͷ��
		for (uint32_t i = 0; i < mLastUsedIndex; ++i)
		{
			if (particles[i].life <= 0.0f)
			{
				mLastUsedIndex = i;
				return i;
			}
		}
		// ��û�ҵ��ʹ�ͷ��ʼ
		mLastUsedIndex = 0;
		return 0;
	}

	void ParticleEmitter::RespawnParticle(const ParticleSystemState& state, Particle& particle) const
	{
		float xRandom = Math::RandomFloat(-0.5f, 0.5f);
		float yRandom = Math::RandomFloat(-0.5f, 0.5f);
		float zRandom = Math::RandomFloat(-0.5f, 0.5f);
		float rColor = Math::RandomFloat(0.5f, 1.0f);
		float gColor = Math::RandomFloat(0.5f, 1.0f);
		float bColor = Math::RandomFloat(0.5f, 1.0f);
		particle.position = state.mCurPos + Vector3(xRandom * state.mOffset.x, yRandom * state.mOffset.y, zRandom * state.mOffset.z);
		particle.color = Vector4(rColor, gColor, bColor, 1.0f);
		particle.life = state.mLifeTime;
		particle.velocity = Math::GetRandomPerpendicular(state.mMoveDir) * 10;
	}
}