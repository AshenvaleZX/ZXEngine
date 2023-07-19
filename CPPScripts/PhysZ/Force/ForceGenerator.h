#pragma once
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class ForceGenerator
		{
		public:
			virtual void UpdateForce(RigidBody* rigidBody, float duration) = 0;
		};
	}
}