#pragma once
#include "../../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class ForceGenerator
		{
		public:
			virtual void UpdateForce(RigidBody* rigidBody, float duration) = 0;
		};
	}
}