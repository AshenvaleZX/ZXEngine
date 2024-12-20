#pragma once
#include "Collider.h"

namespace ZXEngine
{
	class Circle2DCollider : public Collider
	{
	public:
		static ComponentType GetType();

	public:
		PhysZ::CollisionCircle2D* mCollider = nullptr;

		Circle2DCollider();
		~Circle2DCollider();

		virtual ComponentType GetInsType();
		virtual void SynchronizeData();
		virtual void SynchronizeTransform(const Matrix4& transform);
		virtual bool IntersectRay(const PhysZ::Ray& ray);
		virtual bool IntersectRay(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit);
	};
}