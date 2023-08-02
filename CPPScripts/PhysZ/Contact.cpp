#include "Contact.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		void Contact::SetRigidBodies(RigidBody* rigidBody1, RigidBody* rigidBody2)
		{
			mRigidBodies[0] = rigidBody1;
			mRigidBodies[1] = rigidBody2;
		}

		void Contact::SwapRigidBodies()
		{
			mContactNormal *= -1.0f;

			RigidBody* temp = mRigidBodies[0];
			mRigidBodies[0] = mRigidBodies[1];
			mRigidBodies[1] = temp;
		}

		void Contact::UpdateInternalDatas(float duration)
		{
			// Ҫ�����һ���������ײ���������Ե�һ�����������ڣ��ڶ���������Բ�����(ǽ��/����Ȳ����ƶ�����)
			if (!mRigidBodies[0])
			{
				if (!mRigidBodies[1])
				{
					Debug::LogError("Both rigid bodies are null!");
					return;
				}
				else
				{
					SwapRigidBodies();
				}
			}

			// ������ײ����ϵ����������ϵ����ת����
			UpdateOrthogonalBasis();

			// ������ײ����������������λ��
			mRelativeContactPosition[0] = mContactPoint - mRigidBodies[0]->GetPosition();
			if (mRigidBodies[1])
				mRelativeContactPosition[1] = mContactPoint - mRigidBodies[1]->GetPosition();

			// ������������ײ�������ٶ�
			mContactVelocity = CalculateLocalVelocity(0, duration);
			// ����ڶ�������Ҳ�Ǹ���(����ǽ��/����Ȳ����ƶ�����)�����ȥ�ڶ�����������ײ�������ٶȣ��õ������ƶ��еĶ���֮�������ٶ�
			if (mRigidBodies[1])
				mContactVelocity -= CalculateLocalVelocity(1, duration);

			// ���㵱ǰ��ײ�������������ٶȱ仯��
			UpdateDesiredDeltaVelocity(duration);
		}

		void Contact::UpdateOrthogonalBasis()
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

		void Contact::UpdateDesiredDeltaVelocity(float duration)
		{
			// ��ǰ֡���ٶȵ��µ�������ײ���ߵıպ��ٶȱ仯
			float deltaVelocity = 0.0f;

			if (mRigidBodies[0]->GetAwake())
				deltaVelocity += Math::Dot(mRigidBodies[0]->GetLastAcceleration() * duration, mContactNormal);
			if (mRigidBodies[1] && mRigidBodies[1]->GetAwake())
				deltaVelocity -= Math::Dot(mRigidBodies[1]->GetLastAcceleration() * duration, mContactNormal);

			// ����պ��ٶ�̫С�ˣ��ָ�ϵ���Ͱ�0��������ײ��պ��ٶ�Ϊ0��������֮��������ٶȣ����ϵ�һ��
			float restitution = fabsf(mContactVelocity.x) < 0.25f ? 0.0f : mRestitution;

			// ������ײ���߷����ϵ������ٶȱ仯
			// ��������Ǳպ��ٶȵļ��ٶ�(��պ��ٶȵļ��ٶ�Ϊ0)
			// ��ô�ָ�ϵ��Ϊ0ʱ�������Ľ���������尤��һ�𣬱պ��ٶȱ仯����ԭ�պ��ٶȵĸ�����Ҳ���Ǿ�������仯��պ��ٶ�Ϊ0
			// ����ָ�ϵ��Ϊ1ʱ�������Ľ����������������ٶȵ���ʧ�����ٶȴ�С���䣬���Ƿ����෴�ķ�ʽ�˶���Ҳ���Ǳպ��ٶȱ仯Ϊ2������ԭ�պ��ٶ�
			// ���Ǽ��ٶ�֮���ɻָ�ϵ����������ײ��պ��ٶ�(�����ٶ�)����Ҫ���ϼ��ٶȵĴ������������෴����պ��ٶȱ仯����ʱ���Ҫ����������
			mDesiredDeltaVelocity = -mContactVelocity.x - restitution * (mContactVelocity.x - deltaVelocity);
		}

		Vector3 Contact::CalculateLocalVelocity(uint32_t index, float duration)
		{
			auto rigidBody = mRigidBodies[index];

			// �ȼ�����ײ��������ת�������ٶ�
			Vector3 velocity = Math::Cross(rigidBody->GetAngularVelocity(), mRelativeContactPosition[index]);
			// �ټ��ϸ���������ٶȵõ���ײ��ĵ�ǰ�ٶ�(��������ϵ)
			velocity += rigidBody->GetVelocity();

			// ���ٶ�ת������ײ����ϵ(����ײ�����ϵ��ٶȷ��������˶����˴˵��ƶ��ٶȣ�����Ħ������ʱ����Ҫ��)
			Vector3 contactVelocity = mContactToWorld * velocity;

			// �ڲ����Ƿ�������������£����ٶȴ������ٶȱ仯
			Vector3 deltaVelocity = rigidBody->GetLastAcceleration() * duration;
			// ת������ײ����ϵ
			deltaVelocity = mContactToWorld * deltaVelocity;
			// ȥ����ײ�����ϵķ�����ֻ��������ײƽ���ϵ��ٶ�
			deltaVelocity.x = 0.0f;
			// �����ײƽ���ϵ��ٶȱ仯(�����Ħ��������ٶȻ��ں��汻������)
			contactVelocity += deltaVelocity;

			return contactVelocity;
		}
	}
}