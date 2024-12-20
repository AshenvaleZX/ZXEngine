#pragma once
#include "../Component.h"
#include "../../PhysZ/PhysZ.h"

namespace ZXEngine
{
	class Collider : public Component
	{
	public:
		static ComponentType GetType();

	public:
		float mFriction = 0.4f;
		float mBounciness = 0.0f;
		PhysZ::CombineType mFrictionCombine = PhysZ::CombineType::Average;
		PhysZ::CombineType mBounceCombine = PhysZ::CombineType::Average;

		virtual ComponentType GetInsType();
		virtual void SynchronizeData() = 0;
		virtual void SynchronizeTransform(const Matrix4& transform) = 0;
		virtual bool IntersectRay(const PhysZ::Ray& ray) = 0;
		virtual bool IntersectRay(const PhysZ::Ray& ray, PhysZ::RayHitInfo& hit) = 0;
	};
}