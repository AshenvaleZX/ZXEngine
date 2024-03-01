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

	void ZRigidBody::AddForce(const Vector3& force)
	{
		mRigidBody->AddForce(force);
	}

	void ZRigidBody::AddForceAtPoint(const Vector3& force, const Vector3& point)
	{
		mRigidBody->AddForceAtPoint(force, point);
	}

	void ZRigidBody::AddForceAtLocalPoint(const Vector3& force, const Vector3& point)
	{
		mRigidBody->AddForceAtLocalPoint(force, point);
	}

	Vector3 ZRigidBody::GetPosition() const
	{
		return mRigidBody->GetPosition();
	}

	void ZRigidBody::SetPosition(const Vector3& position)
	{
		mRigidBody->SetPosition(position);
	}

	Vector3 ZRigidBody::GetVelocity() const
	{
		return mRigidBody->GetVelocity();
	}

	void ZRigidBody::SetVelocity(const Vector3& velocity)
	{
		mRigidBody->SetVelocity(velocity);
	}
}