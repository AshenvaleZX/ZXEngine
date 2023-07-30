#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void Contact::SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2)
		{
			mRigidBodies[0] = rigidBody1;
			mRigidBodies[1] = rigidBody2;
		}

		void Contact::GenerateOrthogonalBasis()
		{
			// Ĭ����ײ����Ϊ��ײ����ϵ��X�ᣬ������ײ����ϵ��YZ��
			// �����ڼ����ʱ��Ҫ�ر�ע������������ϵ�����⣡����
			Vector3 axisY, axisZ;

			// �ȼ���Y���ټ���Z�ᣬ��ֻȷ����X��������YZ���������ֿ����ԣ����һ�ֶ���
			// ���Կ���������һ�������ײ���߲�˵õ���ײ����ϵY�ᣬ����ѡ����������ϵ��X�����Z��
			// Ϊ�˱�����ײ���߸պú���������ϵ��X�����Z���غϣ�����Ҫ�ж���ײ�������ĸ������
			// �����ײ���ߵ�x������z��������ô��ײ��������������ϵ��X�����������������ϵ��Z������˵õ���ײ����ϵ��Y��
			if (fabsf(mContactNormal.x) > fabsf(mContactNormal.z))
			{
				// ��ʹ��������λ������ˣ�������Ǵ�ֱ��ϵ�����Ҳ�����ǵ�λ����
				// ����������ǰ�����һ����Ҫ�����ݣ��ں���ļ��㷨��ʹ��
				const float s = 1.0f / sqrtf(mContactNormal.x * mContactNormal.x + mContactNormal.y * mContactNormal.y);

				// ����д���Ǽ򻯺�ļ��㣬�ȼ���axisY = Math::Cross(mContactNormal, Vector3(0.0f, 0.0f, 1.0f)).GetNormalized();
				axisY.x =  mContactNormal.y * s;
				axisY.y = -mContactNormal.x * s;
				axisY.z =  0.0f;

				// ����д���Ǽ򻯺�ļ��㣬�ȼ���axisZ = Math::Cross(axisY, ontactNormal);
				axisZ.x =  axisY.y * mContactNormal.z;
				axisZ.y = -axisY.x * mContactNormal.z;
				axisZ.z =  axisY.x * mContactNormal.y - axisY.y * mContactNormal.x;
			}
			// ��ײ��������������ϵZ�����������������ϵX���˵õ���ײ����ϵY��
			else
			{
				const float s = 1.0f / sqrtf(mContactNormal.z * mContactNormal.z + mContactNormal.y * mContactNormal.y);

				// ����д���Ǽ򻯺�ļ��㣬�ȼ���axisY = Math::Cross(Vector3(1.0f, 0.0f, 0.0f), mContactNormal).GetNormalized();
				axisY.x =  0.0f;
				axisY.y = -mContactNormal.z * s;
				axisY.z =  mContactNormal.y * s;

				// ����д���Ǽ򻯺�ļ��㣬�ȼ���axisZ = Math::Cross(ontactNormal, axisY);
				axisZ.x =  mContactNormal.y * axisY.z - mContactNormal.z * axisY.y;
				axisZ.y = -mContactNormal.x * axisY.z;
				axisZ.z =  mContactNormal.x * axisY.y;
			}

			// ��������ϵ��ת����
			mContactToWorld = Matrix3(mContactNormal.x, axisY.x, axisZ.x,
			                          mContactNormal.y, axisY.y, axisZ.y,
				                      mContactNormal.z, axisY.z, axisZ.z);
		}
	}
}