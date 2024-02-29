#include "ZDistanceJoint.h"
#include "../../GameObject.h"

namespace ZXEngine
{
	ComponentType ZDistanceJoint::GetType()
	{
		return ComponentType::DistanceJoint;
	}

	ComponentType ZDistanceJoint::GetInsType()
	{
		return ComponentType::DistanceJoint;
	}

	ZDistanceJoint::ZDistanceJoint()
	{
		mJoint = new PhysZ::DistanceJoint();
	}

	ZDistanceJoint::~ZDistanceJoint()
	{
		delete mJoint;
	}

	void ZDistanceJoint::Init()
	{
		auto body0 = gameObject->GetComponent<ZRigidBody>();
		if (body0 == nullptr)
		{
			Debug::LogError("ZDistanceJoint::Init: No RigidBody found in GameObject");
			return;
		}

		auto connectedGO = GameObject::Find(mConnectedGOPath);
		if (connectedGO == nullptr)
		{
			Debug::LogError("ZDistanceJoint::Init: No connected GameObject found");
			return;
		}

		auto body1 = connectedGO->GetComponent<ZRigidBody>();
		if (body1 == nullptr)
		{
			Debug::LogError("ZDistanceJoint::Init: No RigidBody found in connected GameObject");
			return;
		}

		mJoint->mDistance = mDistance;
		mJoint->SetBodyAndAnchor(body0->mRigidBody, mAnchor, body1->mRigidBody, mOtherAnchor);
	}
}