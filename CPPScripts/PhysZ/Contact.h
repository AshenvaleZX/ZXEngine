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
			// 从碰撞坐标系到世界坐标系的旋转矩阵
			Matrix3 mContactToWorld;

			// 碰撞点相对于两个刚体的位置
			Vector3 mRelativeContactPosition[2];
			// 碰撞点的闭合速度(即两个对象相互接近的速度)
			Vector3 mContactVelocity;
			// 当前碰撞所产生的期望速度变化量
			float mDesiredDeltaVelocity;

			// 交换两个刚体，同时将碰撞法线取反(但是不会更新其它相关变量，如果需要更新手动调用UpdateInternalDatas)
			void SwapRigidBodies();

			// 更新当前碰撞点的各项内部数据
			void UpdateInternalDatas(float duration);
			// 更新碰撞坐标系到世界坐标系的旋转矩阵
			void UpdateOrthogonalBasis();
			void UpdateDesiredDeltaVelocity(float duration);
			
			// 计算第index个刚体相对于碰撞点的速度
			Vector3 CalculateLocalVelocity(uint32_t index, float duration);
		};
	}
}