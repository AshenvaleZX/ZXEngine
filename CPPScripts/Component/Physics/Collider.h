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
	};
}