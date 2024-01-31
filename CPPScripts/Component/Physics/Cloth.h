#pragma once
#include "../Component.h"
#include "../../PhysZ/PhysZ.h"

namespace ZXEngine
{
	class DynamicMesh;
	class PhysZ::RigidBody;
	class PhysZ::CollisionBox;
	class Cloth : public Component
	{
		friend class PhysZ::PScene;
	public:
		static ComponentType GetType();

	public:
		bool mUseGravity = true;
		float mMass = 1.0f;
		float mFriction = 0.5f;
		float mBendStiffness = 1.0f;
		float mStretchStiffness = 1.0f;

		virtual ComponentType GetInsType();

		void Init();

	private:
		DynamicMesh* mDynamicMesh = nullptr;
		vector<pair<PhysZ::RigidBody*, PhysZ::CollisionBox*>> mParticles;

		void AddInternalSpring(uint32_t idx1, uint32_t idx2, float constant);
	};
}