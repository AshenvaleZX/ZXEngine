#include "DistanceJoint.h"
#include "../RigidBody.h"
#include "../Contact.h"
#include "../CollisionData.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		DistanceJoint::DistanceJoint(RigidBody* body0, RigidBody* body1, float distance) :
			Joint(body0, body1), 
			mDistance(distance)
		{}

		DistanceJoint::DistanceJoint(RigidBody* body0, const Vector3& anchor0, RigidBody* body1, const Vector3& anchor1, float distance) :
			Joint(body0, anchor0, body1, anchor1),
			mDistance(distance)
		{}

		void DistanceJoint::Resolve(CollisionData* data)
		{
			if (data->IsFull() || mBodys[0] == nullptr || mBodys[1] == nullptr)
				return;

			const Matrix4& mat0 = mBodys[0]->GetTransform();
			const Matrix4& mat1 = mBodys[1]->GetTransform();

			Vector3 p0 = mat0 * Vector4(mAnchor[0], 1.0);
			Vector3 p1 = mat1 * Vector4(mAnchor[1], 1.0);

			Vector3 p0_to_p1 = p1 - p0;
			float currentDistance = p0_to_p1.GetMagnitude();

			if (currentDistance > mDistance)
			{
				Contact* contact = data->mCurContact;
				contact->mContactNormal = p0_to_p1.GetNormalized();
				contact->mContactPoint = (p0 + p1) * 0.5f;
				contact->mPenetration = currentDistance - mDistance;
				contact->mRestitution = 0.0f;
				contact->mFriction = 1.0f;
				contact->SetRigidBodies(mBodys[0], mBodys[1]);

				data->AddContacts(1);
			}
		}
	}
}