#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void RigidBody::CalculateDerivedData()
		{

		}

		void RigidBody::AddForce(const Vector3& force)
		{
			mForceAccum += force;
		}

		void RigidBody::AddTorque(const Vector3& torque)
		{
			mTorqueAccum += torque;
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

		void RigidBody::SetInertiaTensor(const Matrix3& inertiaTensor)
		{
			mInverseInertiaTensor = Math::Inverse(inertiaTensor);
		}

		void RigidBody::GetInertiaTensor(Matrix3& inertiaTensor) const
		{
			inertiaTensor = Math::Inverse(mInverseInertiaTensor);
		}

		Matrix3 RigidBody::GetInertiaTensor() const
		{
			return Math::Inverse(mInverseInertiaTensor);
		}

		void RigidBody::SetInverseInertiaTensor(const Matrix3& inverseInertiaTensor)
		{
			mInverseInertiaTensor = inverseInertiaTensor;
		}

		void RigidBody::GetInverseInertiaTensor(Matrix3& inverseInertiaTensor) const
		{
			inverseInertiaTensor = mInverseInertiaTensor;
		}

		Matrix3 RigidBody::GetInverseInertiaTensor() const
		{
			return mInverseInertiaTensor;
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
	}
}