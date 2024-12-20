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

	void PlaneCollider::SynchronizeData()
	{
		mCollider->mFriction = mFriction;
		mCollider->mBounciness = mBounciness;
		mCollider->mFrictionCombine = mFrictionCombine;
		mCollider->mBounceCombine = mBounceCombine;
	}

	void PlaneCollider::SynchronizeTransform(const Matrix4& transform)
	{
		mCollider->SynchronizeTransform(transform);
	}

	bool PlaneCollider::IntersectRay(const PhysZ::Ray& ray)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider);
	}

	bool PlaneCollider::IntersectRay(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit)
	{
		return PhysZ::IntersectionDetector::Detect(ray, *mCollider);
	}
}