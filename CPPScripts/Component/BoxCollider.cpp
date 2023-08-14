#include "BoxCollider.h"

namespace ZXEngine
{
	ComponentType BoxCollider::GetType()
	{
		return ComponentType::BoxCollider;
	}

	BoxCollider::BoxCollider()
	{
		mCollider = new PhysZ::CollisionBox();
	}

	BoxCollider::~BoxCollider()
	{
		delete mCollider;
	}

	ComponentType BoxCollider::GetInsType()
	{
		return ComponentType::BoxCollider;
	}
}