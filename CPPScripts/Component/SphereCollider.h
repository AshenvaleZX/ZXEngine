#pragma once
#include "Collider.h"

namespace ZXEngine
{
	class SphereCollider : public Collider
	{
	public:
		static ComponentType GetType();

	public:
		PhysZ::CollisionSphere* mCollider = nullptr;

		SphereCollider();
		~SphereCollider();

		virtual ComponentType GetInsType();
		virtual void SynchronizeData();
	};
}