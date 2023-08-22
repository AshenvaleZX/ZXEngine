#include "Contact.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		Contact::Contact(RigidBody* rigidBody1, RigidBody* rigidBody2)
		{
			mRigidBodies[0] = rigidBody1;
			mRigidBodies[1] = rigidBody2;
		}

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
					// ����˵�����ڵ�ǰ�������ײ���ϣ����ڵ�ǰ������壬������ײ���ߵĵ�λ���Գ�����Ӧ�Ľǳ���
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

		void Contact::ResolveVelocityChange(Vector3 linearVelocityChange[2], Vector3 angularVelocityChange[2])
		{
			// ��ȡ������������������
			Matrix3 inverseInertiaTensor[2];
			mRigidBodies[0]->GetInverseInertiaTensorWorld(inverseInertiaTensor[0]);
			if (mRigidBodies[1])
				mRigidBodies[1]->GetInverseInertiaTensorWorld(inverseInertiaTensor[1]);

			// ��ײ�ռ��еĳ���
			Vector3 impulseContact;
			// ֮ǰ�Ѿ���ͨ��������ײ����ǰ���ٶȣ����ٶȣ��ָ�ϵ������Ϣ��������˱�����ײ������������ٶȱ仯
			// ��������ٶȱ仯�Ǳպ��ٶȵı仯������ֱ�ӵ���������ײ�����ٶȵı仯
			// ����ͨ�������ıպ��ٶȱ仯���Ƴ���Ҫ�ĳ����������ٸ��������������������������ٶȱ仯
			if (Math::Approximately(mFriction, 0.0f))
				impulseContact = CalculateFrictionlessImpulse(inverseInertiaTensor);
			else
				impulseContact = CalculateFrictionImpulse(inverseInertiaTensor);
			
			// ת��������ռ�
			Vector3 impulseWorld = mContactToWorld * impulseContact;

			// ����ǳ�����Jt = r x Jf
			Vector3 angularImpulse = Math::Cross(mRelativeContactPosition[0], impulseWorld);
			// �ǳ�����ʽΪ: Jt = I * w������wΪ���ٶȣ�����w = I^-1 * Jt
			angularVelocityChange[0] = inverseInertiaTensor[0] * angularImpulse;
			// ���Գ�����ʽΪ: J = m * v������vΪ���ٶȣ�����v = J / m
			linearVelocityChange[0] = impulseWorld * mRigidBodies[0]->GetInverseMass();

			// ���¸����ٶ�
			mRigidBodies[0]->AddVelocity(linearVelocityChange[0]);
			mRigidBodies[0]->AddAngularVelocity(angularVelocityChange[0]);

			// ����еڶ������壬�͸��µڶ���������ٶ�
			if (mRigidBodies[1])
			{
				// ���ڵڶ������壬�������������෴��
				Vector3 i2 = Math::Cross(impulseWorld, mRelativeContactPosition[1]);
				angularVelocityChange[1] = inverseInertiaTensor[1] * i2;
				linearVelocityChange[1] = impulseWorld * -mRigidBodies[1]->GetInverseMass();

				mRigidBodies[1]->AddVelocity(linearVelocityChange[1]);
				mRigidBodies[1]->AddAngularVelocity(angularVelocityChange[1]);
			}
		}

		Vector3 Contact::CalculateFrictionImpulse(Matrix3* inverseInertiaTensor)
		{
			// ������ʽΪ: J = m * v
			// ��������ȡ�����ĵ�����ֱ�ӵ��ڵ�λ���������������ٶȱ仯ֵ
			float linearVelocity = mRigidBodies[0]->GetInverseMass();

			// ���������λ��������б�Գƾ���
			// ����ԭ����Ϊv0����������ľ���Ϊm0����ôm0 * v1 = v0 x v1
			Matrix3 impulseToTorque(mRelativeContactPosition[0]);

			// ������߼���ResolvePenetration�������һ������������ResolvePenetration��ֻ��������ײ���߷����ϵ��ٶȣ�������Vector3��Ϊ���
			// ������Ҫ������������������ϵ��ٶȣ�������3���������Matrix3��ʾ(����Vector3ƴ��һ��)
			// 
			// ����֮ǰResolvePenetration������ļ������̣�����Ӧ���ȷֱ����������:
			// Math::Cross(mRelativeContactPosition[i], axisX/axisY/axisZ);
			// ��Ϊ��ǰ��ʵ������ϵ�£�������������ʵ���Ͼ���(1, 0, 0), (0, 1, 0), (0, 0, 1)
			// 
			// ������Խ�������������ĳ˷��������һ�´��룬��Ϊ��������ֿ�����Ľ�����Ͱ�������ŵ�һ����ɾ�����һ���Լ���Ľ����һ����
			// ���������3��������˿��Լ�Ϊ б�Գƾ��� * I (XYZ����������ƴ�������Ǹ���λ����)
			// ��������Ǹ�б�Գƾ�����������ʵ���ϱ���ResolvePenetration��ļ����ֱ��������һ��
			// ��ʱ���б�Գƾ���ĵ�һ�У��ڶ��к͵����зֱ��Ӧ�� mRelativeContactPosition[i] �� axisX/axisY/axisZ �Ĳ�˽��
			// 
			// ������һ�� Math::Cross(mRelativeContactPosition[i], mContactNormal); �Ľ���õ���������ײ���ߵĵ�λ���Գ�����Ӧ�Ľǳ���
			// �����impulseToTorque����ʵ����ֱ�Ӿ͵���������������������ĵ�λ���Գ�����Ӧ�Ľǳ���
			// 
			// Ȼ������Ϳ���ֱ�ӿ�ʼ�ڶ��������������������*�ǳ����õ�һ�����ٶȣ��� w = I^-1 * Jt
			// ���������˼���ǣ�����������������ĵ�λ���Գ����������������ϵĽ��ٶȱ仯
			// x���ϵĽ��ٶȱ仯�ھ����һ�У�y���ڵڶ��У�z���ڵ�����
			// ���Եڶ���Ӧ���������ģ�
			// Matrix3 deltaVelocityWorld = inverseInertiaTensor[0] * impulseToTorque;
			// 
			// Ȼ����������ѽ��ٶ�ת�������ٶȣ�����ͬ����һ�������������ϵ����ٶ�
			// ����Ӧ�����������ϵĽ��ٶȷֱ��� mRelativeContactPosition[i]
			// �������ϵĽ��ٶȱ仯���Ѿ�����һ���ļ��������ˣ�������ĵ�һ����������
			// ����ֱ�� * impulseToTorque�õ��ľ���ĵ�һ�о���x���ϵĽ��ٶȱ仯��(����һ��������ľ���ĵ�һ��)��mRelativeContactPosition[i]��˵Ľ��
			// �ڶ������зֱ���y�ᣬz���ϵĽ��ٶȱ仯����mRelativeContactPosition[i]��˵Ľ��
			// ��������Ľ����˼�ǣ�����������������ĵ�λ���Գ��������ĸ�����ת���µĵ�ǰ��ײ�㴦���������ϵ������ٶȱ仯
			// ������Ӧ���������ģ�
			// deltaVelocityWorld *= impulseToTorque;
			// 
			// ���մ�����ڣ�
			// Matrix3 deltaVelocityWorld = inverseInertiaTensor[0] * impulseToTorque * impulseToTorque;
			// 
			// ���������Cyclone������Ĵ��벻һ�����������г��������������ײЧ����������������ʵ�ʴ��뻹�ǲ�����Cyclone�������
			// Ȼ���Ҳ�û�����Cyclone������Ĵ���Ϊʲô��������
			// ��������Ϊ�������������ǶԳƾ���(��ΪA)��impulseToTorque��б�Գƾ���(��ΪB)����ô�ɾ������ʿɵ� A * B = -(B * A)
			// ����Cyclone�����д�����ҵ�д���ǵȼ۵ģ�����ʵ������Ĺ������������Ǳ�ת��������������ϵ�µģ����Բ����ǶԳƾ�����
			// �ҵ�д���Ͳ��ȼ���Cyclone��������ˣ������ҵ�д�����ˣ����ڻ�û��ͨΪʲôҪд��Cyclone��������������
			Matrix3 deltaVelocityWorld = impulseToTorque * inverseInertiaTensor[0] * impulseToTorque * -1.0f;

			// 
			// ������Cyclone�����ﱾ��������д�ģ�
			// Matrix3 deltaVelocityWorld = impulseToTorque * inverseInertiaTensor[0];
			// Ȼ������������� * -1�������Ҹı���һ�³˷�˳�򣬾Ͳ���*-1�ˣ����Һ�ResolvePenetration��Ĺ���Ҳ��һ��
			// ����֮���Կ�����������ΪimpulseToTorque��һ��б�Գƾ��󣬶����������������ǶԳƾ���(ʵ���Ϻܶ�ʱ���ǶԽǾ���)��������������������Ҳ�϶��ǶԳƾ���
			// ��������£�����б�Գƾ���ΪA���Գƾ���ΪB����ô A * B = -(B * A)������Cyclone�������ʵ�ֺ��������ʵ��ʵ������һ����
			//Matrix3 deltaVelocityWorld = inverseInertiaTensor[0] * impulseToTorque;


			// ����еڶ������壬��ô�Ͱѵڶ���������ٶȱ仯Ҳ�ӽ���
			if (mRigidBodies[1])
			{
				impulseToTorque = Matrix3(mRelativeContactPosition[1]);

				Matrix3 deltaVelocityWorld2 = impulseToTorque * inverseInertiaTensor[1] * impulseToTorque * -1.0f;

				// ������������ٶȱ仯�������
				deltaVelocityWorld += deltaVelocityWorld2;

				// ������������������
				linearVelocity += mRigidBodies[1]->GetInverseMass();
			}

			// ���ٶȱ仯����ת������ײ�ռ�
			Matrix3 deltaVelocityContact = Math::Transpose(mContactToWorld) * deltaVelocityWorld * mContactToWorld;

			// ������Ķ�����ת�����������ٶȱ仯�������ټ��������˶������������ٶȱ仯
			// ���������˶�������ٶȱ仯ͬ��Ҳ�þ������ Matrix3(linearVelocity)
			// ��˼�ǣ�����������������ĵ�λ���Գ��������ĸ��������˶����µĵ�ǰ��ײ�㴦���������ϵ������ٶȱ仯
			// Ȼ����ӵ�ǰ����ת���µ������ٶȱ仯�ϣ��õ��ܵ��ٶȱ仯��
			// 
			// ��ʱ�������������ǣ�����������������ĵ�λ���������������˶�����ת����������µĵ�ǰ��ײ�����������ϵ��ٶȱ仯
			// 
			// �����������������һ�о�������x��ĵ�λ������������ײ���ٶȱ仯��xyz���ϵķ�������Ϊ����ת����x��ĳ�����ֹӰ��x���ϵ��ٶ�
			// �ڶ������о�������y��z��ĵ�λ������������ײ���ٶȱ仯��xyz���ϵķ���
			// 
			// �����������������һ�о�������xyz������ĵ�λ�����ֱ��ܴ�����x���ϵ��ٶȱ仯��
			// �ڶ������о�������xyz������ĵ�λ�����ֱ��ܴ�����y��z���ϵ��ٶȱ仯��
			deltaVelocityContact += Matrix3(linearVelocity);

			// ͨ�������ٶȱ仯���������󣬵õ���λ�ٶȱ仯����Ҫ�ĳ���
			// ����������ֵ���壺
			// ��һ�е�һ�У���x����ÿ�仯һ����λ�ٶ�����Ҫ��x���ϵĳ�����С
			// ��һ�еڶ��У���y����ÿ�仯һ����λ�ٶ�����Ҫ��x���ϵĳ�����С
			// ��һ�е����У���z����ÿ�仯һ����λ�ٶ�����Ҫ��x���ϵĳ�����С
			// �ڶ��е�һ�У���x����ÿ�仯һ����λ�ٶ�����Ҫ��y���ϵĳ�����С
			// �ڶ��еڶ��У���y����ÿ�仯һ����λ�ٶ�����Ҫ��y���ϵĳ�����С
			// �Դ�����......
			Matrix3 impulsePerUnitVelocity = Math::Inverse(deltaVelocityContact);

			// ��������������ײʱҪ�仯���ٶ�(��ײ�ռ�)
			// x��������ײ���߷��򣬱仯�ٶȾ��Ǳ�����ײ���������ģ�����ײ�����ϵı仯�ٶ�
			// y��z�ǻ����Ħ������ƽ�棬�����������ϵ��ٶȻ���ΪĦ����������(��һ����ײ���ܲ�����ȫ������)
			Vector3 velocityToRemove(mDesiredDeltaVelocity, -mContactVelocity.y, -mContactVelocity.z);

			// �����Ƴ������Ǹ��ٶ�����Ҫ�ĳ���(��ײ�ռ�)
			// ���ﰴ��(����m * ����v)�ļ�����򣬽���е�x���� = m00 * x + m01 * y + m02 * z
			// ����������͵Ĵ˾���ʹ������ĺ��壬����е�x�����������Ҫ�����ٶ�v����Ҫ�ĳ�����x���ϵķ�����С
			// ������������ĺ������Ҫ�����ٶ�v����Ҫ�ĳ�����С
			Vector3 impulseContact = impulsePerUnitVelocity * velocityToRemove;

			// ���ǲ��������Ǹ��ٶ�����Ҫ�ĳ�����yzƽ���ϵĴ�С
			float planarImpulse = sqrtf(impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z);

			// ��������жϵ���˼����������᲻�����Ħ���������ṩ�ĳ�����С
			// 
			// ����Ƚϵ���������Ħ������ʽ: Ff = u * Fn������Ff��Ħ������С��u��Ħ��ϵ����Fn����ѹ����С(��Ħ��ƽ�洹ֱ)
			// ����Ħ��������ѹ����С�����ȣ�ϵ��Ϊu����������ʽΪ: J = F * dt������J�ǳ�����С��F������С��dt����������ʱ��
			// ����dt��һ���ģ������������ײ�����ĳ�����x��(��ײ����)�ϵķ�����СҲ�ͺ�yzƽ��(Ħ��ƽ��)�ϵķ�����С�����ȣ�ϵ��Ϊu
			// 
			// ��������ڣ���ô�е����ƾ�Ħ�����������ͷ���������ȣ�Ħ����(yzƽ���ϵķ�������)���㹻��
			// Ҳ���㹻�������ײ��yzƽ���ϵ�����ٶȱ���ȫ�Ƴ���������ֱ�ӷ��ظոռ���Ĳ���velocityToRemove�ٶȵĳ�������
			// 
			// �����൱���Ƕ�Ħ�������Сyzƽ���ϵ�����ٶȣ���ʱ���¼��������С
			// ���������yzƽ���ϵĴ�С���Ƕ�Ħ����(��������)�����ĳ�������������ȫ�Ƴ�yzƽ�������ٶȣ�����ʹ���С
			// ����ģ����Ħ��������ײ���ߵ���ƽ���ϲ���������Ч��
			if (planarImpulse > impulseContact.x * mFriction)
			{
				// Ħ�����㹻�������£�yzƽ���ϵĳ�����ֱ�����Ƴ�yzƽ�������ٶȵĳ���
				// ����������Ħ���������������£�yzƽ���ϵĳ�����ֻ����x�᷽���ϳ�����u��
				// ע����yzƽ���ϵĳ�����x���ϳ�����u����������y���z���ϵĳ�����x���u��
				// ���������������λ������y��z���ϵķ���
				impulseContact.y /= planarImpulse;
				impulseContact.z /= planarImpulse;
				
				// deltaVelocityContact�������˼������������������ĵ�λ���������������˶�����ת����������µĵ�ǰ��ײ�����������ϵ��ٶȱ仯
				// ����ȡ��������x���������ػ�����ϵx��ĵ�λ������������x���ϵ��ٶȱ仯��
				// y���������ػ�����ϵy��ĵ�λ������������x���ϵ��ٶȱ仯
				// z���������ػ�������z��ĵ�λ������������x���ϵ��ٶȱ仯
				Vector3 velocityOnXPerUnitImpulse = deltaVelocityContact.GetRow(0);

				// ���Ǽ���x���ϵĳ���Ϊ��λ������Ҳ����1����ǰ���ֻ���Ħ������£�y���z��ĳ���ʵ�����Ǹ���x��ĳ������������
				// Ҳ����yzƽ���ϵĳ�����С����x���ϵ�u(����Ħ��ϵ��)������Ҳ����u��Ȼ���ٰ�u�ֽ⵽y���z����
				// ����ֽ�������ͨ������planarImpulse����ɣ������impulseContact.y/z�У����Դ�ʱy���z���ϵĳ����ͱ�ʾΪmFriction * impulseContact.y/z
				// x���ϵ��ٶȱ仯��ͬʱ��xyz�������ϵĳ���Ӱ�죬��y���z���ϵĳ�����С����x�������Ħ��ϵ����������������x���ϵ��ٶȱ仯������ȫ��x���ϵĳ�������
				// ����x����Ϊ��λ��������ôx���ϵĵ�λ����������x���ٶȱ仯��ʾ����
				float velocityOnXPerUnitAxisXImpulse = velocityOnXPerUnitImpulse.x
					+ velocityOnXPerUnitImpulse.y * mFriction * impulseContact.y
					+ velocityOnXPerUnitImpulse.z * mFriction * impulseContact.z;

				// mDesiredDeltaVelocity����������x���ϵ��ٶȱ仯������x����ÿ��λ�����������ٶȱ仯�����͵õ�������x������Ҫ�ĳ��С
				impulseContact.x = mDesiredDeltaVelocity / velocityOnXPerUnitAxisXImpulse;

				// u * x����� = yzƽ��������ٳ���yzƽ���ϵĵ�λ������y���z���ϵķ����õ�����y���z��ĳ���
				impulseContact.y *= mFriction * impulseContact.x;
				impulseContact.z *= mFriction * impulseContact.x;
			}

			return impulseContact;
		}

		Vector3 Contact::CalculateFrictionlessImpulse(Matrix3* inverseInertiaTensor)
		{
			// ��λ���Գ�����������ת���µ��ٶȱ仯(���ͼ�ResolvePenetration)
			Vector3 velocityPerUnitImpulseByRotation = Math::Cross(mRelativeContactPosition[0], mContactNormal);
			velocityPerUnitImpulseByRotation = inverseInertiaTensor[0] * velocityPerUnitImpulseByRotation;
			velocityPerUnitImpulseByRotation = Math::Cross(velocityPerUnitImpulseByRotation, mRelativeContactPosition[0]);

			// �ɵ�λ���Գ�����ɵ���ת�����µ���ײ����ٶȱ仯������ײ�����ϵķ���(���ͼ�ResolvePenetration)
			float velocityPerUnitImpulse = Math::Dot(velocityPerUnitImpulseByRotation, mContactNormal);

			// GetInverseMass��ȡ�����ɵ�λ������ɵ������ٶȱ仯��(���ͼ�ResolvePenetration)
			// ��Ӻ�õ��ɵ�λ������ɵ������˶��ͽ��˶�����������ٶȱ仯��
			velocityPerUnitImpulse += mRigidBodies[0]->GetInverseMass();

			// ����еڶ������壬��ô�Ͱѵڶ���������ٶȱ仯��Ҳ����
			if (mRigidBodies[1])
			{
				// ����ͬ��
				Vector3 v2 = Math::Cross(mRelativeContactPosition[1], mContactNormal);
				v2 = inverseInertiaTensor[1] * v2;
				v2 = Math::Cross(v2, mRelativeContactPosition[1]);

				velocityPerUnitImpulse += Math::Dot(v2, mContactNormal);

				velocityPerUnitImpulse += mRigidBodies[1]->GetInverseMass();
			}

			// �����Ѿ��õ��˵�λ������ɵ��ٶȱ仯��velocityPerUnitImpulse
			// �������������ٶȱ仯������velocityPerUnitImpulse�Ϳ��Եõ���ɶ�Ӧ�ٶȱ仯����Ҫ�ĳ���
			// ��ײ�����ĳ���������������ײ���ߵģ����ﷵ�ص�������ײ�ռ��µĳ���������ֻ����x��(�涨��ײ��������ײ�ռ�����x�᷽��)����
			return Vector3(mDesiredDeltaVelocity / velocityPerUnitImpulse, 0.0f, 0.0f);
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
			Vector3 contactVelocity = Math::Transpose(mContactToWorld) * velocity;

			// �ڲ����Ƿ�������������£����ٶȴ������ٶȱ仯
			Vector3 deltaVelocity = rigidBody->GetLastAcceleration() * duration;
			// ת������ײ����ϵ
			deltaVelocity = Math::Transpose(mContactToWorld) * deltaVelocity;
			// ȥ����ײ�����ϵķ�����ֻ��������ײƽ���ϵ��ٶ�
			deltaVelocity.x = 0.0f;
			// �����ײƽ���ϵ��ٶȱ仯(�����Ħ��������ٶȻ��ں��汻������)
			contactVelocity += deltaVelocity;

			return contactVelocity;
		}
	}
}