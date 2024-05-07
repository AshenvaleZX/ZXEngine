#pragma once
#include "Collider.h"

namespace ZXEngine
{
	class BoxCollider : public Collider
	{
	public:
		static ComponentType GetType();

	public:
		PhysZ::CollisionBox* mCollider = nullptr;

		BoxCollider();
		~BoxCollider();

		virtual ComponentType GetInsType();
		virtual void SynchronizeData();
		virtual void SynchronizeTransform(const Matrix4& transform);
		virtual bool IntersectRay(const PhysZ::Ray& ray);
	};
}