#include "PlaneCollider.h"

namespace ZXEngine
{
	ComponentType PlaneCollider::GetType()
	{
		return ComponentType::PlaneCollider;
	}

	PlaneCollider::PlaneCollider()
	{
		mCollider = new PhysZ::CollisionPlane();
	}

	PlaneCollider::~PlaneCollider()
	{
		delete mCollider;
	}

	ComponentType PlaneCollider::GetInsType()
	{
		return ComponentType::PlaneCollider;
	}
}