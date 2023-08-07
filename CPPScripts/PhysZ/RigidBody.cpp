#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void RigidBody::Integrate(float duration)
		{
			if (!mIsAwake)
				return;

			// 通过合力计算加速度
			mLastAcceleration = mAcceleration;
			mLastAcceleration += mForceAccum * mInverseMass;

			// 通过合力矩计算角加速度
			Vector3 angularAcceleration = mWorldInverseInertiaTensor * mTorqueAccum;

			// 根据加速度更新速度
			mVelocity += mLastAcceleration * duration;

			// 根据角加速度更新角速度
			mAngularVelocity += angularAcceleration * duration;

			// 施加阻尼效果
			mVelocity *= pow(mLinearDamping, duration);
			mAngularVelocity *= pow(mAngularDamping, duration);

			// 根据速度移动位置
			mPosition += mVelocity * duration;

			// 根据角速度更新旋转状态
			mRotation.RotateByVector(mAngularVelocity, duration);

			// 更新其它数据
			CalculateDerivedData();

			// 清除累积的作用力和力矩
			ClearAccumulators();
		}

		void RigidBody::CalculateDerivedData()
		{
			UpdateTransform();
			UpdateWorldInertiaTensor();
		}

		void RigidBody::UpdateTransform()
		{
			mTransform = mRotation.ToMatrix();
			mTransform = Math::Translate(mTransform, mPosition);
		}

		void RigidBody::UpdateWorldInertiaTensor()
		{
			// 根据当前的transform信息，重新计算世界坐标系下的惯性张量
			Matrix3 rot(mTransform);
			// 惯性张量的变换公式为: I' = R * I * R^T，只需要考虑旋转变换即可
			mWorldInverseInertiaTensor = rot * mLocalInverseInertiaTensor * Math::Transpose(rot);
		}

		void RigidBody::AddForce(const Vector3& force)
		{
			mForceAccum += force;
			mIsAwake = true;
		}

		void RigidBody::AddForceAtPoint(const Vector3& force, const Vector3& point)
		{
			// 相对于质心的位置
			auto pos = point - mPosition;

			mForceAccum += force;
			mTorqueAccum += Math::Cross(pos, force);

			mIsAwake = true;
		}

		void RigidBody::AddForceAtLocalPoint(const Vector3& force, const Vector3& point)
		{
			AddForceAtPoint(force, mTransform * Vector4(point, 1.0f));
		}

		void RigidBody::AddTorque(const Vector3& torque)
		{
			mTorqueAccum += torque;
			mIsAwake = true;
		}

		void RigidBody::ClearAccumulators()
		{
			mForceAccum.Clear();
			mTorqueAccum.Clear();
		}

		bool RigidBody::IsInfiniteMass() const
		{
			return mInverseMass <= 0.0f;
		}

		void RigidBody::SetAwake(bool awake)
		{
			mIsAwake = awake;
		}

		bool RigidBody::GetAwake() const
		{
			return mIsAwake;
		}

		Matrix4 RigidBody::GetTransform() const
		{
			return mTransform;
		}

		void RigidBody::SetMass(float mass)
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

		float RigidBody::GetMass() const
		{
			return 1.0f / mInverseMass;
		}

		void RigidBody::SetInverseMass(float inverseMass)
		{
			mInverseMass = inverseMass;
		}

		float RigidBody::GetInverseMass() const
		{
			return mInverseMass;
		}

		void RigidBody::SetLinearDamping(float damping)
		{
			mLinearDamping = damping;
		}

		float RigidBody::GetLinearDamping() const
		{
			return mLinearDamping;
		}

		void RigidBody::SetPosition(const Vector3& position)
		{
			mPosition = position;
		}

		void RigidBody::GetPosition(Vector3& position) const
		{
			position = mPosition;
		}

		Vector3 RigidBody::GetPosition() const
		{
			return mPosition;
		}

		void RigidBody::SetVelocity(const Vector3& velocity)
		{
			mVelocity = velocity;
		}

		void RigidBody::GetVelocity(Vector3& velocity) const
		{
			velocity = mVelocity;
		}

		Vector3 RigidBody::GetVelocity() const
		{
			return mVelocity;
		}

		void RigidBody::AddVelocity(const Vector3& deltaVelocity)
		{
			mVelocity += deltaVelocity;
		}

		void RigidBody::SetAcceleration(const Vector3& acceleration)
		{
			mAcceleration = acceleration;
		}

		void RigidBody::GetAcceleration(Vector3& acceleration) const
		{
			acceleration = mAcceleration;
		}

		Vector3 RigidBody::GetAcceleration() const
		{
			return mAcceleration;
		}

		void RigidBody::GetLastAcceleration(Vector3& acceleration) const
		{
			acceleration = mLastAcceleration;
		}

		Vector3 RigidBody::GetLastAcceleration() const
		{
			return mLastAcceleration;
		}

		void RigidBody::SetInertiaTensor(const Matrix3& inertiaTensor)
		{
			mLocalInverseInertiaTensor = Math::Inverse(inertiaTensor);
		}

		void RigidBody::GetInertiaTensor(Matrix3& inertiaTensor) const
		{
			inertiaTensor = Math::Inverse(mLocalInverseInertiaTensor);
		}

		Matrix3 RigidBody::GetInertiaTensor() const
		{
			return Math::Inverse(mLocalInverseInertiaTensor);
		}

		void RigidBody::SetInverseInertiaTensor(const Matrix3& inverseInertiaTensor)
		{
			mLocalInverseInertiaTensor = inverseInertiaTensor;
		}

		void RigidBody::GetInverseInertiaTensor(Matrix3& inverseInertiaTensor) const
		{
			inverseInertiaTensor = mLocalInverseInertiaTensor;
		}

		Matrix3 RigidBody::GetInverseInertiaTensor() const
		{
			return mLocalInverseInertiaTensor;
		}

		void RigidBody::GetInverseInertiaTensorWorld(Matrix3& inverseInertiaTensor) const
		{
			inverseInertiaTensor = mWorldInverseInertiaTensor;
		}

		Matrix3 RigidBody::GetInverseInertiaTensorWorld() const
		{
			return mWorldInverseInertiaTensor;
		}

		void RigidBody::SetAngularDamping(float damping)
		{
			mAngularDamping = damping;
		}

		float RigidBody::GetAngularDamping() const
		{
			return mAngularDamping;
		}

		void RigidBody::SetRotation(const Quaternion& rotation)
		{
			mRotation = rotation;
		}

		void RigidBody::GetRotation(Quaternion& rotation) const
		{
			rotation = mRotation;
		}

		Quaternion RigidBody::GetRotation() const
		{
			return mRotation;
		}

		void RigidBody::SetAngularVelocity(const Vector3& angularVelocity)
		{
			mAngularVelocity = angularVelocity;
		}

		void RigidBody::GetAngularVelocity(Vector3& angularVelocity) const
		{
			angularVelocity = mAngularVelocity;
		}

		Vector3 RigidBody::GetAngularVelocity() const
		{
			return mAngularVelocity;
		}

		void RigidBody::AddAngularVelocity(const Vector3& deltaAngularVelocity)
		{
			mAngularVelocity += deltaAngularVelocity;
		}
	}
}