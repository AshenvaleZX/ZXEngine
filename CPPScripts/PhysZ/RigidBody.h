#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody
		{
		public:
			RigidBody() {};
			~RigidBody() {};

			void CalculateDerivedData();
			// 直接在质心添加一个作用力(世界坐标系)
			void AddForce(const Vector3& force);
			// 在某个点(世界坐标系)上添加作用力(世界坐标系)
			void AddForceAtPoint(const Vector3& force, const Vector3& point);
			// 在某个点(局部坐标系)上添加作用力(世界坐标系)
			void AddForceAtLocalPoint(const Vector3& force, const Vector3& point);
			// 添加一个力矩(世界坐标系)
			void AddTorque(const Vector3& torque);
			// 清除当前累计的作用力和力矩
			void ClearAccumulators();
			// 判断质量是否为无穷大
			bool IsInfiniteMass() const;
			
			Matrix4 GetTransform() const;

			// ---------- 线性运动 ----------

			void SetMass(float mass);
			float GetMass() const;
			void SetInverseMass(float inverseMass);
			float GetInverseMass() const;

			void SetLinearDamping(float damping);
			float GetLinearDamping() const;

			void SetPosition(const Vector3& position);
			void GetPosition(Vector3& position) const;
			Vector3 GetPosition() const;

			void SetVelocity(const Vector3& velocity);
			void GetVelocity(Vector3& velocity) const;
			Vector3 GetVelocity() const;

			void SetAcceleration(const Vector3& acceleration);
			void GetAcceleration(Vector3& acceleration) const;
			Vector3 GetAcceleration() const;

			// ---------- 旋转运动 ----------

			void SetInertiaTensor(const Matrix3& inertiaTensor);
			void GetInertiaTensor(Matrix3& inertiaTensor) const;
			Matrix3 GetInertiaTensor() const;
			void SetInverseInertiaTensor(const Matrix3& inverseInertiaTensor);
			void GetInverseInertiaTensor(Matrix3& inverseInertiaTensor) const;
			Matrix3 GetInverseInertiaTensor() const;

			void SetAngularDamping(float damping);
			float GetAngularDamping() const;

			void SetRotation(const Quaternion& rotation);
			void GetRotation(Quaternion& rotation) const;
			Quaternion GetRotation() const;

			void SetAngularVelocity(const Vector3& angularVelocity);
			void GetAngularVelocity(Vector3& angularVelocity) const;
			Vector3 GetAngularVelocity() const;

		private:
			// 质量的倒数(0代表质量无穷大，无视任何作用力)
			float mInverseMass = 1.0f;
			// 线性运动阻尼系数(1表示无阻尼)
			float mLinearDamping = 1.0f;
			// 位置
			Vector3 mPosition;
			// 速度
			Vector3 mVelocity;
			// 加速度
			Vector3 mAcceleration;
			// 累积作用力
			Vector3 mForceAccum;

			// Local惯性张量(以矩阵形式表达,存储逆矩阵方便计算)
			Matrix3 mLocalInverseInertiaTensor;
			// World惯性张量(以矩阵形式表达,存储逆矩阵方便计算)
			Matrix3 mWorldInverseInertiaTensor;
			// 旋转运动阻尼系数(1表示无阻尼)
			float mAngularDamping = 1.0f;
			// 旋转
			Quaternion mRotation;
			// 角速度
			Vector3 mAngularVelocity;
			// 累积力矩
			Vector3 mTorqueAccum;

			// local到world空间的变换
			Matrix4 mTransform;

			void UpdateWorldInertiaTensor();
		};
	}
}