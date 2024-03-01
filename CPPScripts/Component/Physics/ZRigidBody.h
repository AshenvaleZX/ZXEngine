#pragma once
#include "../Component.h"
#include "../../PhysZ/PhysZ.h"

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

		void AddForce(const Vector3& force);
		void AddForceAtPoint(const Vector3& force, const Vector3& point);
		void AddForceAtLocalPoint(const Vector3& force, const Vector3& point);

		Vector3 GetPosition() const;
		void SetPosition(const Vector3& position);

		Vector3 GetVelocity() const;
		void SetVelocity(const Vector3& velocity);
	};
}