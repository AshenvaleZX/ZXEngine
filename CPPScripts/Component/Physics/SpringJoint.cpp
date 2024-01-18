#include "SpringJoint.h"
#include "../../GameObject.h"

namespace ZXEngine
{
	ComponentType SpringJoint::GetType()
	{
		return ComponentType::SpringJoint;
	}

	ComponentType SpringJoint::GetInsType()
	{
		return ComponentType::SpringJoint;
	}

	void SpringJoint::Init()
	{
		mConnectedGO = GameObject::Find(mConnectedGOPath);
		
		if (mConnectedGO == nullptr) 
		{
			Debug::LogError("SpringJoint::SetConnectedGameObject: Cannot find GameObject with path: " + mConnectedGOPath);
			return;
		}

		auto zRigidBody = gameObject->GetComponent<ZRigidBody>();
		if (zRigidBody == nullptr)
		{
			Debug::LogError("SpringJoint::SetConnectedGameObject: Cannot find ZRigidBody on GameObject with path: " + gameObject->name);
			return;
		}

		auto otherZRigidBody = mConnectedGO->GetComponent<ZRigidBody>();
		if (otherZRigidBody == nullptr)
		{
			Debug::LogError("SpringJoint::SetConnectedGameObject: Cannot find ZRigidBody on GameObject with path: " + mConnectedGO->name);
			return;
		}

		auto fgSpring = new PhysZ::FGSpring(
			mAnchor, mOtherAnchor, 
			otherZRigidBody->mRigidBody,
			mSpringConstant, mRestLength
		);

		mRigidBody = zRigidBody->mRigidBody;
		mRigidBody->AddForceGenerator(fgSpring);
	}
}