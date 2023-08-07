#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void RigidBody::Integrate(float duration)
		{
			if (!mIsAwake)
				return;

			// ͨ������������ٶ�
			mLastAcceleration = mAcceleration;
			mLastAcceleration += mForceAccum * mInverseMass;

			// ͨ�������ؼ���Ǽ��ٶ�
			Vector3 angularAcceleration = mWorldInverseInertiaTensor * mTorqueAccum;

			// ���ݼ��ٶȸ����ٶ�
			mVelocity += mLastAcceleration * duration;

			// ���ݽǼ��ٶȸ��½��ٶ�
			mAngularVelocity += angularAcceleration * duration;

			// ʩ������Ч��
			mVelocity *= pow(mLinearDamping, duration);
			mAngularVelocity *= pow(mAngularDamping, duration);

			// �����ٶ��ƶ�λ��
			mPosition += mVelocity * duration;

			// ���ݽ��ٶȸ�����ת״̬
			mRotation.RotateByVector(mAngularVelocity, duration);

			// ������������
			CalculateDerivedData();

			// ����ۻ���������������
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
			// ���ݵ�ǰ��transform��Ϣ�����¼�����������ϵ�µĹ�������
			Matrix3 rot(mTransform);
			// ���������ı任��ʽΪ: I' = R * I * R^T��ֻ��Ҫ������ת�任����
			mWorldInverseInertiaTensor = rot * mLocalInverseInertiaTensor * Math::Transpose(rot);
		}

		void RigidBody::AddForce(const Vector3& force)
		{
			mForceAccum += force;
			mIsAwake = true;
		}

		void RigidBody::AddForceAtPoint(const Vector3& force, const Vector3& point)
		{
			// ��������ĵ�λ��
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
				// �����������0����������쳣������������Ϊ�����
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