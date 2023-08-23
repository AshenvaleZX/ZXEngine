#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class RigidBody;
		class Contact
		{
			friend class ContactResolver;
		public:
			// ��ײ������
			Vector3 mContactPoint;
			// ��ײ����
			Vector3 mContactNormal;
			// ��ײ���(����ײ���ཻʱ������)
			float mPenetration = 0.0f;
			// �ָ�ϵ��
			float mRestitution = 0.0f;
			// Ħ��ϵ��
			float mFriction = 0.0f;

			Contact(RigidBody* rigidBody1 = nullptr, RigidBody* rigidBody2 = nullptr);

			void SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2);

		private:
			// ��ײ����Ӧ�ĸ���
			RigidBody* mRigidBodies[2];
			// ����ײ����ϵ����������ϵ����ת����
			Matrix3 mContactToWorld;

			// ��ײ����������������λ��
			Vector3 mRelativeContactPosition[2];
			// ��ײ��ıպ��ٶ�(�����������໥�ӽ����ٶȣ���ײ�ռ�)
			Vector3 mContactVelocity;
			// ��ǰ��ײ�������������ٶȱ仯��(�պ��ٶ�����ײ�����ϵı仯��)
			float mDesiredDeltaVelocity = 0.0f;

			// ����Ħ��ϵ���ͻָ�ϵ��
			void UpdateCoefficient();
			// �����������壬ͬʱ����ײ����ȡ��(���ǲ������������ر����������Ҫ�����ֶ�����UpdateInternalDatas)
			void SwapRigidBodies();
			// ������ײ�еĸ���״̬���������һ��������Awake����һ��Ҳ����Awake
			void MatchAwakeState();

			// ������ײ��͸��ͨ���ƶ�����ת�������壬�������������󲻽���
			// Ȼ��ͨ��ǰ����ָ��������ش�����Ϣ
			// PS: �˺�����Ȼ���ڴ����ཻ��Ҳ��ı�����λ�ú���ת״̬�����ǲ�����ֱ�Ӹı�mPenetration�������ɵ��õĵط�ȥ����mPenetration
			void ResolvePenetration(Vector3 linearChange[2], Vector3 angularChange[2], float penetration);
			// ������ײ�е��ٶȱ仯����������������ٶȱ仯����Ȼ��ͨ����������
			void ResolveVelocityChange(Vector3 linearVelocityChange[2], Vector3 angularVelocityChange[2]);

			// ������Ħ��������µĳ���
			Vector3 CalculateFrictionImpulse(Matrix3* inverseInertiaTensor);
			// ������Ħ��������µĳ���
			Vector3 CalculateFrictionlessImpulse(Matrix3* inverseInertiaTensor);

			// ���µ�ǰ��ײ��ĸ����ڲ�����
			void UpdateInternalDatas(float duration);
			// ������ײ����ϵ����������ϵ����ת����
			void UpdateOrthogonalBasis();
			// ���㵱ǰ��ײ�������������ٶȱ仯��(�պ��ٶ�)
			void UpdateDesiredDeltaVelocity(float duration);
			
			// �����index�������������ײ����ٶ�
			Vector3 CalculateLocalVelocity(uint32_t index, float duration);
		};
	}
}