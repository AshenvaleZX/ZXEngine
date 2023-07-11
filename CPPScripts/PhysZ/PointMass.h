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

			// ģ�������������ʵ�״̬
			void Integrate(float duration);
			// ���һ��������
			void AddForce(const Vector3& force);
			// �����ǰ�ĺ���
			void ClearForce();
			// �ж������Ƿ�Ϊ�����
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
			// �����ĵ���
			// 0������������������κ�������
			float mInverseMass = 1.0f;
			// ����ϵ����1��ʾ������
			float mDamping = 1.0f;
			// λ��
			Vector3 mPosition;
			// �ٶ�
			Vector3 mVelocity;
			// ���ٶ�
			Vector3 mAcceleration;
			// ����
			Vector3 mForceAccum;
		};
	}
}