#include "SphereCollider.h"

namespace ZXEngine
{
	ComponentType SphereCollider::GetType()
	{
		return ComponentType::SphereCollider;
	}

	SphereCollider::SphereCollider()
	{
		mCollider = new PhysZ::CollisionSphere();
	}

	SphereCollider::~SphereCollider()
	{
		delete mCollider;
	}

	ComponentType SphereCollider::GetInsType()
	{
		return ComponentType::SphereCollider;
	}
}