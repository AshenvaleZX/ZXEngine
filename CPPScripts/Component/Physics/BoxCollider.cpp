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

	void BoxCollider::SynchronizeData()
	{
		mCollider->mFriction = mFriction;
		mCollider->mBounciness = mBounciness;
		mCollider->mFrictionCombine = mFrictionCombine;
		mCollider->mBounceCombine = mBounceCombine;
	}

	void BoxCollider::SynchronizeTransform(const Matrix4& transform)
	{
		mCollider->SynchronizeTransform(transform);
	}

	bool BoxCollider::IntersectRay(const PhysZ::Ray& ray)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider);
	}

	bool BoxCollider::IntersectRay(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider);
	}
}