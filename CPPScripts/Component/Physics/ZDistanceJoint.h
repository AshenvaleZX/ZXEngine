#pragma once
#include "../Component.h"
#include "../../PhysZ/PhysZ.h"

namespace ZXEngine
{
	class ZDistanceJoint : public Component
	{
	public:
		static ComponentType GetType();

	public:
		Vector3 mAnchor;
		Vector3 mOtherAnchor;
		float mDistance = 0.0f;
		string mConnectedGOPath;

		ZDistanceJoint();
		~ZDistanceJoint();

		virtual ComponentType GetInsType();

		void Init();

	private:
		PhysZ::DistanceJoint* mJoint = nullptr;
	};
}