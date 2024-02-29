#pragma once
#include "Joint.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class DistanceJoint : public Joint
		{
		public:
			float mDistance = 0.0f;

			DistanceJoint() {};
			DistanceJoint(RigidBody* body0, RigidBody* body1, float distance);
			DistanceJoint(RigidBody* body0, const Vector3& anchor0, RigidBody* body1, const Vector3& anchor1, float distance);

			virtual void Resolve(CollisionData* data) override;
		};
	}
}