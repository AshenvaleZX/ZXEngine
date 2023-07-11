#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class PointMass
		{
		public:
			PointMass() {};
			~PointMass() {};

			// 模拟积分运算更新质点状态
			void Integrate(float duration);
			// 添加一个作用力
			void AddForce(const Vector3& force);
			// 清除当前的合力
			void ClearForce();
			// 判断质量是否为无穷大
			bool IsInfiniteMass() const;

			void SetMass(float mass);
			float GetMass() const;
			void SetInverseMass(float inverseMass);
			float GetInverseMass() const;

			void SetDamping(float damping);
			float GetDamping() const;

			void SetPosition(const Vector3& position);
			void SetPosition(float x, float y, float z);
			void GetPosition(Vector3& position) const;
			Vector3 GetPosition() const;

			void SetVelocity(const Vector3& velocity);
			void SetVelocity(float x, float y, float z);
			void GetVelocity(Vector3& velocity) const;
			Vector3 GetVelocity() const;

			void SetAcceleration(const Vector3& acceleration);
			void SetAcceleration(float x, float y, float z);
			void GetAcceleration(Vector3& acceleration) const;
			Vector3 GetAcceleration() const;

		protected:
			// 质量的倒数
			// 0代表质量无穷大，无视任何作用力
			float mInverseMass = 1.0f;
			// 阻尼系数，1表示无阻尼
			float mDamping = 1.0f;
			// 位置
			Vector3 mPosition;
			// 速度
			Vector3 mVelocity;
			// 加速度
			Vector3 mAcceleration;
			// 合力
			Vector3 mForceAccum;
		};
	}
}