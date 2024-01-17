#include "ZRigidBody.h"

namespace ZXEngine
{
	ComponentType ZRigidBody::GetType()
	{
		return ComponentType::RigidBody;
	}

	ZRigidBody::ZRigidBody()
	{
		mRigidBody = new PhysZ::RigidBody();
	}

	ZRigidBody::~ZRigidBody()
	{
		delete mRigidBody;
	}

	ComponentType ZRigidBody::GetInsType()
	{
		return ComponentType::RigidBody;
	}
}