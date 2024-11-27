#pragma once
#include "../pubh.h"
#include "ParticleSystemDefinition.h"

namespace ZXEngine
{
	class ParticleEmitter
	{
	public:
		ParticleEmitter() = default;
		~ParticleEmitter() = default;

		void Emit(const ParticleSystemState& state, vector<Particle>& particles);

	private:
		float mLastGenTime = 0;
		uint32_t mLastUsedIndex = 0;

		uint32_t GetUnusedParticleIndex(const ParticleSystemState& state, vector<Particle>& particles);
		void RespawnParticle(const ParticleSystemState& state, Particle& particle) const;
	};
}