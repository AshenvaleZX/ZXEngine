#pragma once
#include "../../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		enum class ForceGeneratorType
		{
			None,
			Gravity,
			Spring,
		};

		class RigidBody;
		class ForceGenerator
		{
		public:
			ForceGeneratorType mType;

			virtual ~ForceGenerator() = default;

			virtual void UpdateForce(RigidBody* rigidBody, float duration) = 0;
		};
	}
}