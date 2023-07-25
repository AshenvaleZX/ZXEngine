#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class Contact
		{
		public:
			// 碰撞点坐标
			Vector3 mContactPoint;
			// 碰撞法线
			Vector3 mContactNormal;
			// 碰撞深度(有碰撞或相交时是正数)
			float mPenetration;
			// 恢复系数
			float mRestitution;
			// 摩擦系数
			float mFriction;

			void SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2);

		private:
			// 碰撞所对应的刚体
			RigidBody* mRigidBodies[2];
		};
	}
}