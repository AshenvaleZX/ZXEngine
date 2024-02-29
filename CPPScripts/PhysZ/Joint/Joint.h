#pragma once
#include "../../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class CollisionData;
		class Joint
		{
		public:
			static vector<Joint*> allJoints;

		public:
			Joint();
			Joint(RigidBody* body0, RigidBody* body1);
			Joint(RigidBody* body0, const Vector3& anchor0, RigidBody* body1, const Vector3& anchor1);
			virtual ~Joint();

			virtual void Resolve(CollisionData* data) = 0;

			void SetBodys(RigidBody* body0, RigidBody* body1);
			void SetAnchor(const Vector3& anchor0, const Vector3& anchor1);
			void SetBodyAndAnchor(RigidBody* body0, const Vector3& anchor0, RigidBody* body1, const Vector3& anchor1);

		protected:
			Vector3 mAnchor[2];
			RigidBody* mBodys[2] = { nullptr, nullptr };
		};
	}
}