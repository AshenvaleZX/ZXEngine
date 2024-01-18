#pragma once
#include "../Component.h"
#include "../../PhysZ/PhysZ.h"

namespace ZXEngine
{
	class GameObject;
	class SpringJoint : public Component
	{
	public:
		static ComponentType GetType();

	public:
		string mConnectedGOPath;
		float mRestLength = 0.0f;
		float mSpringConstant = 0.0f;
		Vector3 mAnchor = Vector3::Zero;
		Vector3 mOtherAnchor = Vector3::Zero;

		SpringJoint() {};
		~SpringJoint() {};

		virtual ComponentType GetInsType();

		void Init();

	private:
		GameObject* mConnectedGO = nullptr;
		PhysZ::RigidBody* mRigidBody = nullptr;
	};
}