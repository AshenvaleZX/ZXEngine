#include "Circle2DCollider.h"

namespace ZXEngine
{
	ComponentType Circle2DCollider::GetType()
	{
		return ComponentType::Circle2DCollider;
	}

	Circle2DCollider::Circle2DCollider()
	{
		mCollider = new PhysZ::CollisionCircle2D();
	}

	Circle2DCollider::~Circle2DCollider()
	{
		delete mCollider;
	}

	ComponentType Circle2DCollider::GetInsType()
	{
		return ComponentType::Circle2DCollider;
	}

	void Circle2DCollider::SynchronizeData()
	{
		mCollider->mFriction = mFriction;
		mCollider->mBounciness = mBounciness;
		mCollider->mFrictionCombine = mFrictionCombine;
		mCollider->mBounceCombine = mBounceCombine;
	}

	void Circle2DCollider::SynchronizeTransform(const Matrix4& transform)
	{
		mCollider->SynchronizeTransform(transform);
	}

	bool Circle2DCollider::IntersectRay(const PhysZ::Ray& ray)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider);
	}

	bool Circle2DCollider::IntersectRay(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider, hit);
	}
}