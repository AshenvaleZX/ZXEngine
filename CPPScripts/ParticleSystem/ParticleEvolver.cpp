#include "ParticleEvolver.h"
#include "../Time.h"

namespace ZXEngine
{
	void ParticleEvolver::Update(const ParticleSystemState& state, vector<Particle>& particles)
	{
		float dt = Time::deltaTime;
		for (uint32_t i = 0; i < state.mMaxParticleNum; i++)
		{
			Particle& p = particles[i];
			p.life -= dt;
			if (p.life > 0.0f)
			{
				p.position += p.velocity * dt;
				p.color.a -= dt * 1;
			}
		}
	}
}