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

		void Contact::MatchAwakeState()
		{
			// ֻ��������ײ�����Ǹ���ʱ�ſ�����Ҫ����(ǰ��Ĵ���ᱣ֤�����������ʱ���������һ�������ǷǸ��壬һ���ڵڶ���λ��)
			// �������һ�������ǷǸ���(����/ǽ��)����ô��һ�������Ƿ�Awake���У���Ϊֻ����һ������Awakeһ������Sleep��ʱ�����Ҫ����Sleep���Ǹ�
			if (!mRigidBodies[1])
				return;

			bool awake0 = mRigidBodies[0]->GetAwake();
			bool awake1 = mRigidBodies[1]->GetAwake();

			// ���һ��Awake��һ��Sleep������Sleep���Ǹ�
			if (awake0 ^ awake1)
			{
				if (awake0)
					mRigidBodies[1]->SetAwake(true);
				else
					mRigidBodies[0]->SetAwake(true);
			}
		}

		void Contact::ResolvePenetration(Vector3 linearChange[2], Vector3 angularChange[2], float penetration)
		{
			// ����������Ҫͨ�������˶����ƶ��ľ���
			float linearMove[2]  = { 0.0f, 0.0f };
			// ����������Ҫͨ����ת�˶����ƶ��ľ���
			float angularMove[2] = { 0.0f, 0.0f };

			// �ܹ��Դ�С
			float totalInertia = 0.0f;
			// ÿ��λ�������µ���������������˶�������������ײ����ٶȱ仯(���������������Դ�С)
			float linearInertia[2]  = { 0.0f, 0.0f };
			// ÿ��λ�������µ������������ת������������ײ����ٶȱ仯(���������������Դ�С)
			float angularInertia[2] = { 0.0f, 0.0f };

			for (uint32_t i = 0; i < 2; i++)
			{
				if (mRigidBodies[i])
				{
					// ��������ϵ�µĹ����������������
					Matrix3 inverseInertiaTensor;
					mRigidBodies[i]->GetInverseInertiaTensorWorld(inverseInertiaTensor);

					// ��һ�м�����ǵ�λ���Գ���(Linear Impulse)��ɵĽǳ���(Angular Impulse)
					// 
					// ��������ײǰ�����ı仯
					// ���Գ�����ʽΪ: Jf = m * (v - v')
					// ����mΪ����������v��v'Ϊ��ײǰ���ٶ�
					// �ǳ�����ʽΪ: Jt = I * (w - w')
					// ����IΪ�������������w��w'Ϊ��ײǰ����ٶ�
					// ���Գ����ͽǳ����Ĺ�ϵΪ: Jt = r x Jf
					// ����r���������õ㵽��ת�������
					// 
					// �������Գ����ķ��������ײ���ߵķ���������ײ���߿��Ե����ǵ�λ���Գ���
					// Ȼ���������Ĺ�ʽ����õ��ڵ�ǰ�������£���λ���Գ�����Ӧ�ĵ�λ�ǳ���
					Vector3 velocityPerUnitImpulseWorld = Math::Cross(mRelativeContactPosition[i], mContactNormal);
					// Ȼ��ǳ������Թ�������(�����������������*�ǳ���)�õ����ٶȵı仯��
					// ����õ��ľ��ǵ�λ���Գ��������Ľ��ٶȱ仯
					velocityPerUnitImpulseWorld = inverseInertiaTensor * velocityPerUnitImpulseWorld;
					// �ٶ� = ���ٶ� x ���õ㵽��ת�������
					// �����ٴβ�����õ㵽��ת����������õ��˵�λ���Գ�����������ת���µ��ٶȱ仯
					velocityPerUnitImpulseWorld = Math::Cross(velocityPerUnitImpulseWorld, mRelativeContactPosition[i]);

					// Ŀǰ�������������ٶ�����������ϵ����Ҫת������ײ����ϵ
					// Vector3 velocityPerUnitImpulseContact = Math::Inverse(mContactToWorld) * velocityPerUnitImpulseWorld;
					// 
					// Ȼ����������ֻ������ײ�����ϵ��ٶȣ�����ֻȡx������ֵ
					// float angularComponent = velocityPerUnitImpulseContact.x;
					// 
					// Ȼ����Ϊֻȡ�����ս����x��������x����ʵ���Ͼ���Math::Inverse(mContactToWorld)����ĵ�һ�к�velocityPerUnitImpulseWorld���
					// ���������2�д�����Ծ���Ϊ������һ��
					// �������������ֵ����˼���ǣ��ɵ�λ���Գ�����ɵ���ת�����µ���ײ����ٶȱ仯������ײ�����ϵķ���
					// ���������������Ϊһ�ֺ�����ת���Դ�С����ֵ��ֵԽ�����ԽС
					angularInertia[i] = Math::Dot(velocityPerUnitImpulseWorld, mContactNormal);

					// Ȼ���ټ��㵥λ���������������˶����ٶȱ仯
					// ��λ����ֱ�ӳ��������Ϳ��Եõ��ٶȱ仯��
					// Ȼ����Ϊ�������������ײ���߷������Եõ����ٶȱ仯��Ҳ����Ҫȡ����
					// ����ֱ����1�����������ɣ�Ҳ������������
					// ���ֵ����˼���ǣ��ɵ�λ������ɵ������ٶȱ仯��(����ײ�����ϵķ���)
					// �����ֵҲ�ǿ���������ʾ���Դ�С�ģ�ֵԽ�������˶�����ԽС
					linearInertia[i] = mRigidBodies[i]->GetInverseMass();

					// Ȼ����ӵ��ܵĹ�����
					totalInertia += linearInertia[i] + angularInertia[i];
				}
			}

			for (uint32_t i = 0; i < 2; i++)
			{
				if (mRigidBodies[i])
				{
					// ��������ķ������ͬʱ�����������˶��ͽ��˶�
					// �����������������˶��ͽ��˶���������Եĵ���������
					// �����������һ����Ҫ��������ײ����֮�以��Զ��penetration��С�ľ���
					// �������һ����Ϊ�Ĳ��֣�������������Ե������˶��ͽ��˶�
					// �������һ������Ϊ�Ǹ��壬��ֻ��2���֣����������������˶��ͽ��˶�
					// ������4��(����2��)�˶���ɵ�Զ��������������penetration
					// �����ж�һ�·������������ƶ��������෴�ģ�����Զ��(�������Ե�һ��������ӽ�����)
					float sign = (i == 0) ? 1.0f : -1.0f;
					// ��i��������Ҫͨ�������˶���Զ��ľ���
					linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);
					// ��i��������Ҫͨ�����˶���Զ��ľ���
					angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
					
					// ������Ҫ���ƽ��˶��ľ��룬��Ҫ̫���ˣ���Ȼ�����ʱ����ܻ���ֹ�����ת
					// ��������˹�����ת�����ܻᵼ�����������޷����룬����ԭ��ײ������˵���������һ�߲���������ײ��
					// ����Ҫ���Ʒ����˶�����ת��ռ�ı���
					// ���Ʒ�ʽ�����кܶ��֣��������ƹ̶��ƶ����룬���߹̶���ת�Ƕ�
					// ����ͨ������Ĵ�С�����������ƶ��ľ��룬Խ�������������ת�ƶ��ľ���ҲԽ��
					// ��Ϊ�ƶ���ͬ����Ļ���Խ�������ʵ����Ҫ��ת�ĽǶ�ԽС
					float limit = 0.2f * mRelativeContactPosition[i].GetMagnitude();
					// ������˶��ľ��볬�������ƣ�����Ҫ���·���
					if (fabsf(angularMove[i]) > limit)
					{
						// �ȼ������ƶ����룬���ֵ����
						float totalMove = linearMove[i] + angularMove[i];

						// Ȼ��ѽ��˶��ƶ��ľ������Ϊ���Ƶ������룬ͬʱע�Ᵽ�ַ��Ų���
						if (angularMove[i] >= 0)
							angularMove[i] = limit;
						else 
							angularMove[i] = -limit;

						// Ȼ�����¼��������˶�����
						linearMove[i] = totalMove - angularMove[i];
					}

					// ���˶�������ͨ����ת��ʵ�ַ���ģ������Ѿ�֪������Ҫͨ�����˶���Զ��ľ���
					// Ȼ����Ҫ������Ҫ��ת���ٽǶȣ����ܴﵽ����ƶ�����
					// �����Ҫ��ת�ƶ��ľ���ǳ�С����ֱ�Ӻ��Ե�
					if (angularMove[i] < 0.0001f)
					{
						angularChange[i].Clear();
					}
					else
					{
						// ͬ��һ��forѭ���еĽ��ͣ������ڵ�ǰ�������£���λ���Գ�����Ӧ�ĵ�λ�ǳ���
						Vector3 angularImpulsePerUnitLinearImpulse = Math::Cross(mRelativeContactPosition[i], mContactNormal);
						// ͬ�ϣ���λ���Գ��������Ľ��ٶȱ仯
						Vector3 angularVelocityPerLinearImpulse = mRigidBodies[i]->GetInverseInertiaTensorWorld() * angularImpulsePerUnitLinearImpulse;
						// ������͹����angularInertia����˼�ǣ��ɵ�λ���Գ�����ɵ���ת�����µ���ײ����ٶȱ仯������ײ�����ϵķ���
						// ��ô�á���λ���Գ��������Ľ��ٶȱ仯�����ԡ���λ���Գ�����ɵ���ת�����µ���ײ����ٶȱ仯������ײ�����ϵķ�����
						// �͵õ��ˡ�������ײ���߷���ÿ�仯һ����λ�ٶ���Ҫ�仯�Ľ��ٶȡ�
						// ��� ���ٶ� = n���ٶȣ���ô����ͬʱ����ʱ��t��ʽ���ɳ������� �Ƕ� = n����
						// ����Ҳ���൱�ڵõ��ˡ�������ײ����ÿ�ƶ�һ����λ������Ҫ��ת�ĽǶȡ�
						Vector3 angularPerMove = angularVelocityPerLinearImpulse / angularInertia[i];
						// Ȼ�������Ҫ�ƶ��ľ��룬�͵õ�����Ҫ��ת�ĽǶ�
						angularChange[i] = angularPerMove * angularMove[i];
					}

					// �����˶����ƶ���ֱ������Ҫ�ľ��������ײ���߼���
					linearChange[i] = mContactNormal * linearMove[i];

					// ��������λ��(ͨ�������˶�Զ����һ����ײ����)
					Vector3 pos;
					mRigidBodies[i]->GetPosition(pos);
					pos += linearChange[i];
					mRigidBodies[i]->SetPosition(pos);

					// ����������ת(ͨ�����˶�Զ����һ����ײ����)
					Quaternion q;
					mRigidBodies[i]->GetRotation(q);
					q.RotateByVector(angularChange[i]);
					mRigidBodies[i]->SetRotation(q);

					// ������崦��Sleep״̬���ֶ����ýӿڸ����������
					// ����˺����޷�����������壬������һ֡��������ͬ���ཻ���(penetration)
					if (!mRigidBodies[i]->GetAwake())
						mRigidBodies[i]->CalculateDerivedData();
				}
			}
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