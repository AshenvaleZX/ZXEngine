#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class Contact
		{
			friend class ContactResolver;
		public:
			// 碰撞点坐标
			Vector3 mContactPoint;
			// 碰撞法线
			Vector3 mContactNormal;
			// 碰撞深度(有碰撞或相交时是正数)
			float mPenetration = 0.0f;
			// 恢复系数
			float mRestitution = 0.0f;
			// 摩擦系数
			float mFriction = 0.0f;

			Contact(RigidBody* rigidBody1 = nullptr, RigidBody* rigidBody2 = nullptr);

			void SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2);

		private:
			// 碰撞所对应的刚体
			RigidBody* mRigidBodies[2];
			// 从碰撞坐标系到世界坐标系的旋转矩阵
			Matrix3 mContactToWorld;

			// 碰撞点相对于两个刚体的位置
			Vector3 mRelativeContactPosition[2];
			// 碰撞点的闭合速度(即两个对象相互接近的速度，碰撞空间)
			Vector3 mContactVelocity;
			// 当前碰撞所产生的期望速度变化量(闭合速度在碰撞法线上的变化量)
			float mDesiredDeltaVelocity = 0.0f;

			// 更新摩擦系数和恢复系数
			void UpdateCoefficient();
			// 交换两个刚体，同时将碰撞法线取反(但是不会更新其它相关变量，如果需要更新手动调用UpdateInternalDatas)
			void SwapRigidBodies();
			// 更新碰撞中的刚体状态，如果其中一个刚体是Awake，另一个也必须Awake
			void MatchAwakeState();

			// 处理碰撞穿透，通过移动和旋转两个刚体，尽量让两个对象不交叉
			// 然后通过前两个指针参数返回处理信息
			// PS: 此函数虽然是在处理相交，也会改变刚体的位置和旋转状态，但是并不会直接改变mPenetration，而是由调用的地方去调整mPenetration
			void ResolvePenetration(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);
			// 处理碰撞中的速度变化，计算两个刚体的速度变化量，然后通过参数返回
			void ResolveVelocityChange(Vector3 linearVelocityChange[2], Vector3 angularVelocityChange[2]);

			// 计算有摩擦力情况下的冲量
			Vector3 CalculateFrictionImpulse(Matrix3* inverseInertiaTensor);
			// 计算无摩擦力情况下的冲量
			Vector3 CalculateFrictionlessImpulse(Matrix3* inverseInertiaTensor);

			// 更新当前碰撞点的各项内部数据
			void UpdateInternalDatas(float duration);
			// 更新碰撞坐标系到世界坐标系的旋转矩阵
			void UpdateOrthogonalBasis();
			// 计算当前碰撞所产生的期望速度变化量(闭合速度)
			void UpdateDesiredDeltaVelocity(float duration);
			
			// 计算第index个刚体相对于碰撞点的速度
			Vector3 CalculateLocalVelocity(uint32_t index, float duration);
		};
	}
}