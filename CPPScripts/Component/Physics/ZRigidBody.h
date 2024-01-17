#pragma once
#include "Component.h"
#include "../PhysZ/PhysZ.h"

namespace ZXEngine
{
	class ZRigidBody : public Component
	{
	public:
		static ComponentType GetType();

	public:
		bool mUseGravity = true;
		PhysZ::RigidBody* mRigidBody = nullptr;

		ZRigidBody();
		~ZRigidBody();

		virtual ComponentType GetInsType();
	};
}