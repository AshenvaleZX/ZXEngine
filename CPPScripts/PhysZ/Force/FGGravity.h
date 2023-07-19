#pragma once
#include "ForceGenerator.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class FGGravity : public ForceGenerator
		{
		public:
			FGGravity(const Vector3& gravity) : gravity(gravity) {};

			virtual void UpdateForce(RigidBody* rigidBody, float duration);

		private:
			Vector3 gravity;
		};
	}
}