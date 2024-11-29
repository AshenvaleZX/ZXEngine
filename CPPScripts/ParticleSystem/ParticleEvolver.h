#pragma once
#include "../pubh.h"
#include "ParticleSystemDefinition.h"

namespace ZXEngine
{
	class ParticleEvolver
	{
	public:
		struct KeyFloat
		{
			float t;
			float value;
		};

		struct KeyVector3
		{
			float t;
			Vector3 value;
		};

	public:
		vector<KeyFloat> mAlphaKeys;
		vector<KeyVector3> mRGBKeys;

		ParticleEvolver() = default;
		~ParticleEvolver() = default;

		void Update(const ParticleSystemState& state, vector<Particle>& particles);

	private:
		template<typename T1, typename T2>
		void KeyLerp(float t, const vector<T1>& keys, T2& value)
		{
			if (keys.size() > 1)
			{
				for (size_t i = 0; i < keys.size() - 1; i++)
				{
					const auto& key = keys[i];
					const auto& nextKey = keys[i + 1];

					if (t >= key.t && t < nextKey.t)
					{
						value = Math::Lerp(key.value, nextKey.value, (t - key.t) / (nextKey.t - key.t));
						break;
					}
				}
			}
		}
	};

	StaticReflection
	(
		ParticleEvolver::KeyFloat,
		Fields
		(
			Field(&ParticleEvolver::KeyFloat::t),
			Field(&ParticleEvolver::KeyFloat::value)
		)
	)

	StaticReflection
	(
		ParticleEvolver::KeyVector3,
		Fields
		(
			Field(&ParticleEvolver::KeyVector3::t),
			Field(&ParticleEvolver::KeyVector3::value)
		)
	)

	StaticReflection
	(
		ParticleEvolver,
		Fields
		(
			Field(&ParticleEvolver::mAlphaKeys),
			Field(&ParticleEvolver::mRGBKeys)
		)
	)
}