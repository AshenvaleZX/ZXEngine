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

			// 更新刚体的位置和旋转
			void Integrate(float duration);
			// 计算需要跟随刚体变化而变化的相关数据
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

			void SetAwake(bool awake);
			bool GetAwake() const;
			void SetCanSleep(bool canSleep);
			bool GetCanSleep() const;
			
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
			void AddVelocity(const Vector3& deltaVelocity);

			void SetAcceleration(const Vector3& acceleration);
			void GetAcceleration(Vector3& acceleration) const;
			Vector3 GetAcceleration() const;

			void GetLastAcceleration(Vector3& acceleration) const;
			Vector3 GetLastAcceleration() const;

			// ---------- 旋转运动 ----------

			void SetInertiaTensor(const Matrix3& inertiaTensor);
			void GetInertiaTensor(Matrix3& inertiaTensor) const;
			Matrix3 GetInertiaTensor() const;
			void SetInverseInertiaTensor(const Matrix3& inverseInertiaTensor);
			void GetInverseInertiaTensor(Matrix3& inverseInertiaTensor) const;
			Matrix3 GetInverseInertiaTensor() const;
			void GetInverseInertiaTensorWorld(Matrix3& inverseInertiaTensor) const;
			Matrix3 GetInverseInertiaTensorWorld() const;

			void SetAngularDamping(float damping);
			float GetAngularDamping() const;

			void SetRotation(const Quaternion& rotation);
			void GetRotation(Quaternion& rotation) const;
			Quaternion GetRotation() const;

			void SetAngularVelocity(const Vector3& angularVelocity);
			void GetAngularVelocity(Vector3& angularVelocity) const;
			Vector3 GetAngularVelocity() const;
			void AddAngularVelocity(const Vector3& deltaAngularVelocity);

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
			// 上一帧的加速度
			Vector3 mLastAcceleration;
			// 累积作用力
			Vector3 mForceAccum;

			// Local惯性张量(以矩阵形式表达,存储逆矩阵方便计算)
			Matrix3 mLocalInverseInertiaTensor;
			// World惯性张量(以矩阵形式表达,存储逆矩阵方便计算)
			Matrix3 mWorldInverseInertiaTensor;
			// 旋转运动阻尼系数(1表示无阻尼)
			float mAngularDamping = 0.95f;
			// 旋转
			Quaternion mRotation;
			// 角速度
			Vector3 mAngularVelocity;
			// 累积力矩
			Vector3 mTorqueAccum;

			// local到world空间的变换
			Matrix4 mTransform;

			// 睡眠状态的刚体不进行物理更新
			bool mIsAwake = true;
			// 刚体是否可以进入睡眠状态
			// 一般默认为true，某些特殊对象，比如玩家操控的，可能随时手动产生作用力的对象可以设置为false
			bool mCanSleep = true;
			// 刚体当前的运动量，包含线性速度和角速度，无实际物理意义，仅用于评估运动状态
			float mMotion = 0.0f;

			void UpdateTransform();
			void UpdateWorldInertiaTensor();
		};
	}
}