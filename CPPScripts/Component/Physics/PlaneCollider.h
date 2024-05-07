#pragma once
#include "Collider.h"

namespace ZXEngine
{
	class PlaneCollider : public Collider
	{
	public:
		static ComponentType GetType();

	public:
		PhysZ::CollisionPlane* mCollider = nullptr;

		PlaneCollider();
		~PlaneCollider();

		virtual ComponentType GetInsType();
		virtual void SynchronizeData();
		virtual void SynchronizeTransform(const Matrix4& transform);
		virtual bool IntersectRay(const PhysZ::Ray& ray);
	};
}