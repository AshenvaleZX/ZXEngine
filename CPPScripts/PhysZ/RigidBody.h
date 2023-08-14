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

			// ���¸����λ�ú���ת
			void Integrate(float duration);
			// ������Ҫ�������仯���仯���������
			void CalculateDerivedData();
			// ֱ�����������һ��������(��������ϵ)
			void AddForce(const Vector3& force);
			// ��ĳ����(��������ϵ)�����������(��������ϵ)
			void AddForceAtPoint(const Vector3& force, const Vector3& point);
			// ��ĳ����(�ֲ�����ϵ)�����������(��������ϵ)
			void AddForceAtLocalPoint(const Vector3& force, const Vector3& point);
			// ���һ������(��������ϵ)
			void AddTorque(const Vector3& torque);
			// �����ǰ�ۼƵ�������������
			void ClearAccumulators();
			// �ж������Ƿ�Ϊ�����
			bool IsInfiniteMass() const;

			void SetAwake(bool awake);
			bool GetAwake() const;
			void SetCanSleep(bool canSleep);
			bool GetCanSleep() const;
			
			Matrix4 GetTransform() const;

			// ---------- �����˶� ----------

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

			// ---------- ��ת�˶� ----------

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
			// �����ĵ���(0������������������κ�������)
			float mInverseMass = 1.0f;
			// �����˶�����ϵ��(1��ʾ������)
			float mLinearDamping = 1.0f;
			// λ��
			Vector3 mPosition;
			// �ٶ�
			Vector3 mVelocity;
			// ���ٶ�
			Vector3 mAcceleration;
			// ��һ֡�ļ��ٶ�
			Vector3 mLastAcceleration;
			// �ۻ�������
			Vector3 mForceAccum;

			// Local��������(�Ծ�����ʽ���,�洢����󷽱����)
			Matrix3 mLocalInverseInertiaTensor;
			// World��������(�Ծ�����ʽ���,�洢����󷽱����)
			Matrix3 mWorldInverseInertiaTensor;
			// ��ת�˶�����ϵ��(1��ʾ������)
			float mAngularDamping = 0.95f;
			// ��ת
			Quaternion mRotation;
			// ���ٶ�
			Vector3 mAngularVelocity;
			// �ۻ�����
			Vector3 mTorqueAccum;

			// local��world�ռ�ı任
			Matrix4 mTransform;

			// ˯��״̬�ĸ��岻�����������
			bool mIsAwake = true;
			// �����Ƿ���Խ���˯��״̬
			// һ��Ĭ��Ϊtrue��ĳЩ������󣬱�����Ҳٿصģ�������ʱ�ֶ������������Ķ����������Ϊfalse
			bool mCanSleep = true;
			// ���嵱ǰ���˶��������������ٶȺͽ��ٶȣ���ʵ���������壬�����������˶�״̬
			float mMotion = 0.0f;

			void UpdateTransform();
			void UpdateWorldInertiaTensor();
		};
	}
}