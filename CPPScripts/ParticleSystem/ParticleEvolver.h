#pragma once
#include "../pubh.h"
#include "ParticleSystemDefinition.h"

namespace ZXEngine
{
	class ParticleEvolver
	{
	public:
		ParticleEvolver() = default;
		~ParticleEvolver() = default;

		void Update(const ParticleSystemState& state, vector<Particle>& particles);
	};
}