#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class Contact
		{
		public:
			// ��ײ������
			Vector3 mContactPoint;
			// ��ײ����
			Vector3 mContactNormal;
			// ��ײ���(����ײ���ཻʱ������)
			float mPenetration;
			// �ָ�ϵ��
			float mRestitution;
			// Ħ��ϵ��
			float mFriction;

			void SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2);

		private:
			// ��ײ����Ӧ�ĸ���
			RigidBody* mRigidBodies[2];
		};
	}
}