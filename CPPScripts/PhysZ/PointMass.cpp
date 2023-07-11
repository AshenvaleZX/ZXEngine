#include "PointMass.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void PointMass::Integrate(float duration)
		{
			// 若质量无穷大，则无视受力
			if (mInverseMass <= 0.0f) return;

			assert(duration > 0.0);

			// 用速度更新位置
			mPosition += mVelocity * duration;

			// 用合力更新加速度
			Vector3 resultingAcc = mAcceleration + mForceAccum * mInverseMass;

			// 用加速度更新速度
			mVelocity += resultingAcc * duration;

			// 计算阻尼带来的速度衰减
			mVelocity *= pow(mDamping, duration);

			// 清除合力
			ClearForce();
		}

		void PointMass::AddForce(const Vector3& force)
		{
			mForceAccum += force;
		}

		void PointMass::ClearForce()
		{
			mForceAccum.Clear();
		}

		bool PointMass::IsInfiniteMass() const
		{
			return mInverseMass <= 0.0f;
		}

		void PointMass::SetMass(float mass)
		{
			if (mass <= 0.0f)
			{
				// 质量必须大于0，否则输出异常并将质量设置为无穷大
				Debug::LogError("Mass must be greater than zero.");
				mInverseMass = 0.0f;
			}
			else
			{
				mInverseMass = 1.0f / mass;
			}
		}

		float PointMass::GetMass() const
		{
			return 1.0f / mInverseMass;
		}
		
		void PointMass::SetInverseMass(float inverseMass)
		{
			mInverseMass = inverseMass;
		}

		float PointMass::GetInverseMass() const
		{
			return mInverseMass;
		}

		void PointMass::SetDamping(float damping)
		{
			mDamping = damping;
		}

		float PointMass::GetDamping() const
		{
			return mDamping;
		}

		void PointMass::SetPosition(const Vector3& position)
		{
			mPosition = position;
		}

		void PointMass::SetPosition(float x, float y, float z)
		{
			mPosition.x = x;
			mPosition.y = y;
			mPosition.z = z;
		}

		void PointMass::GetPosition(Vector3& position) const
		{
			position = mPosition;
		}

		Vector3 PointMass::GetPosition() const
		{
			return mPosition;
		}

		void PointMass::SetVelocity(const Vector3& velocity)
		{
			mVelocity = velocity;
		}

		void PointMass::SetVelocity(float x, float y, float z)
		{
			mVelocity.x = x;
			mVelocity.y = y;
			mVelocity.z = z;
		}

		void PointMass::GetVelocity(Vector3& velocity) const
		{
			velocity = mVelocity;
		}

		Vector3 PointMass::GetVelocity() const
		{
			return mVelocity;
		}

		void PointMass::SetAcceleration(const Vector3& acceleration)
		{
			mAcceleration = acceleration;
		}

		void PointMass::SetAcceleration(float x, float y, float z)
		{
			mAcceleration.x = x;
			mAcceleration.y = y;
			mAcceleration.z = z;
		}

		void PointMass::GetAcceleration(Vector3& acceleration) const
		{
			acceleration = mAcceleration;
		}

		Vector3 PointMass::GetAcceleration() const
		{
			return mAcceleration;
		}
	}
}