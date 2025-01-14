#include "ParticleEmitter.h"
#include "../Time.h"

namespace ZXEngine
{
	void ParticleEmitter::Update(const ParticleSystemState& state, vector<Particle>& particles)
	{
		// 粒子生产的时间间隔
		float interval = 1.0f / state.mEmissionState.mRate;
		float curTime = Time::curTime;
		uint32_t genNum = 0;
		if (mLastGenTime == 0)
		{
			// 第一帧先初始化数据
			mLastGenTime = curTime;
		}
		else
		{
			// 上一次生产粒子的时间
			float delta = curTime - mLastGenTime;
			// 如果相差时间大于了生产间隔，那么根据相差的这段时间，计算这一帧应该生产的数量
			if (delta > interval)
				genNum = static_cast<uint32_t>(delta / interval);
			// 如果这一帧生产了粒子，那么更新生产时间
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
		// 从上一个用过的向后找
		for (uint32_t i = mLastUsedIndex; i < state.mMaxParticleNum; ++i)
		{
			if (particles[i].life <= 0.0f)
			{
				mLastUsedIndex = i;
				return i;
			}
		}
		// 没找到就从头找
		for (uint32_t i = 0; i < mLastUsedIndex; ++i)
		{
			if (particles[i].life <= 0.0f)
			{
				mLastUsedIndex = i;
				return i;
			}
		}
		// 都没找到就从头开始
		mLastUsedIndex = 0;
		return 0;
	}

	void ParticleEmitter::RespawnParticle(const ParticleSystemState& state, Particle& particle) const
	{
		particle.life = state.mLifeTime;

		float xRandom = Math::RandomFloat(-0.5f, 0.5f);
		float yRandom = Math::RandomFloat(-0.5f, 0.5f);
		float zRandom = Math::RandomFloat(-0.5f, 0.5f);
		particle.position = state.mCurPos + Vector3(xRandom * state.mOffset.x, yRandom * state.mOffset.y, zRandom * state.mOffset.z);

		if (state.mEmissionState.mRandomColor)
		{
			float rColor = Math::RandomFloat(0.0f, 1.0f);
			float gColor = Math::RandomFloat(0.0f, 1.0f);
			float bColor = Math::RandomFloat(0.0f, 1.0f);
			particle.color = Vector4(rColor, gColor, bColor, 1.0f);
		}
		else
		{
			particle.color = state.mEmissionState.mColor;
		}

		const Vector3& front = (state.mCurRot * state.mEmissionState.mRotation).ToMatrix3() * Vector3::Up;

		if (state.mEmissionState.mShape == ParticleEmissionState::Shape::Sphere)
		{
			float r1 = Math::RandomFloat(0.0f, 1.0f);
			float r2 = Math::RandomFloat(0.0f, 1.0f);

			float theta = 2.0f * Math::PI * r1;
			float phi = acos(2.0f * r2 - 1.0f);

			float x = sin(phi) * cos(theta);
			float y = sin(phi) * sin(theta);
			float z = cos(phi);
			Vector3 dir(x, y, z);
			
			particle.velocity = dir * state.mEmissionState.mSpeed;
		}
		else if (state.mEmissionState.mShape == ParticleEmissionState::Shape::Hemisphere)
		{
			float angle = Math::RandomFloat(0.0f, 1.0f);

			Vector3 dir = Math::GetRandomPerpendicular(front);
			dir = Math::Slerp(front, dir, angle);
			dir.Normalize();

			particle.velocity = dir * state.mEmissionState.mSpeed;
		}
		else if (state.mEmissionState.mShape == ParticleEmissionState::Shape::Cone)
		{
			float angle = Math::RandomFloat(0.0f, state.mEmissionState.mAngle / 90.0f);

			Vector3 dir = Math::GetRandomPerpendicular(front);
			dir = Math::Slerp(front, dir, angle);
			dir.Normalize();

			particle.velocity = dir * state.mEmissionState.mSpeed;
		}
		else if (state.mEmissionState.mShape == ParticleEmissionState::Shape::Circle)
		{
			Vector3 dir = Math::GetRandomPerpendicular(front);
			particle.velocity = dir * state.mEmissionState.mSpeed;
		}
		else if (state.mEmissionState.mShape == ParticleEmissionState::Shape::Box)
		{
			Vector3 dir = Vector3(Math::RandomFloat(-1.0f, 1.0f), Math::RandomFloat(-1.0f, 1.0f), Math::RandomFloat(-1.0f, 1.0f));
			dir.Normalize();
			particle.velocity = dir * state.mEmissionState.mSpeed;
		}
		else // ParticleEmissionState::Shape::Line
		{
			particle.velocity = front * state.mEmissionState.mSpeed;
		}
	}
}