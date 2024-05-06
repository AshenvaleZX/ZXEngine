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

	void SphereCollider::SynchronizeData()
	{
		mCollider->mFriction = mFriction;
		mCollider->mBounciness = mBounciness;
		mCollider->mFrictionCombine = mFrictionCombine;
		mCollider->mBounceCombine = mBounceCombine;
	}

	bool SphereCollider::IntersectRay(const PhysZ::Ray& ray)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider);
	}
}