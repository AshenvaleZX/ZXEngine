#include "ContactResolver.h"
#include "Contact.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		ContactResolver::ContactResolver(uint32_t maxIterations, float velocityEpsilon, float positionEpsilon) :
			mMaxVelocityIterations(maxIterations),
			mMaxPositionIterations(maxIterations),
			mCurVelocityIterations(0),
			mCurPositionIterations(0),
			mVelocityEpsilon(velocityEpsilon), 
			mPositionEpsilon(positionEpsilon)
		{}

		ContactResolver::ContactResolver(uint32_t maxVelocityIterations, uint32_t maxPositionIterations, float velocityEpsilon, float positionEpsilon) :
			mMaxVelocityIterations(maxVelocityIterations),
			mMaxPositionIterations(maxPositionIterations),
			mCurVelocityIterations(0),
			mCurPositionIterations(0),
			mVelocityEpsilon(velocityEpsilon), 
			mPositionEpsilon(positionEpsilon)
		{}

		bool ContactResolver::IsValid()
		{
			return (mMaxVelocityIterations > 0) &&
				(mMaxPositionIterations > 0) &&
				(mVelocityEpsilon >= 0.0f) &&
				(mPositionEpsilon >= 0.0f);
		}

		void ContactResolver::SetMaxIterations(uint32_t maxIterations)
		{
			mMaxVelocityIterations = maxIterations;
			mMaxPositionIterations = maxIterations;
		}

		void ContactResolver::SetVelocityEpsilon(float velocityEpsilon)
		{
			mVelocityEpsilon = velocityEpsilon;
		}

		void ContactResolver::SetPositionEpsilon(float positionEpsilon)
		{
			mPositionEpsilon = positionEpsilon;
		}

		void ContactResolver::ResolveContacts(Contact* contacts, uint32_t numContacts, float duration)
		{
			if (numContacts == 0)
				return;
			if (!IsValid())
				return;

			// ׼����ײ����
			PrepareContacts(contacts, numContacts, duration);
			// ������ײ�ཻ
			AdjustPositions(contacts, numContacts, duration);
			// ������ײ�ٶȱ仯
			AdjustVelocities(contacts, numContacts, duration);
		}

		void ContactResolver::PrepareContacts(Contact* contacts, uint32_t numContacts, float duration)
		{
			Contact* lastContact = contacts + numContacts;
			for (Contact* contact = contacts; contact < lastContact; contact++)
			{
				contact->UpdateInternalDatas(duration);
			}
		}

		void ContactResolver::AdjustPositions(Contact* contacts, uint32_t numContacts, float duration)
		{
			// ��ֱ���ƶ�����
			Vector3 linearChange[2];
			// ת���Ƕ�
			Vector3 angularChange[2];
			// ����ཻ���
			float maxPenetration;

			// ��ײ���λ�ñ仯��(��������ǰ���������������ѭ��������������Ȼ����ջ��Ƶ������������)
			Vector3 deltaPosition;
			// ��ǰ�������ײ����
			uint32_t idx;

			mCurPositionIterations = 0;
			while (mCurPositionIterations < mMaxPositionIterations)
			{
				idx = numContacts;
				maxPenetration = mPositionEpsilon;
				// �ҵ��ཻ�������ײ
				for (uint32_t i = 0; i < numContacts; i++)
				{
					if (contacts[i].mPenetration > maxPenetration)
					{
						maxPenetration = contacts[i].mPenetration;
						idx = i;
					}
				}

				// ˵��û����Ҫ������ཻ�ˣ�ֱ���˳�
				if (idx == numContacts)
					break;

				// ���ѿ��ܴ�������״̬�ĸ���
				contacts[idx].MatchAwakeState();

				// �����ཻ������ȡ�ཻ������Ϣ�������ʵ�ʸı����������λ�ú���ת״̬
				contacts[idx].ResolvePenetration(linearChange, angularChange, maxPenetration);

				// Ȼ��������е���ײ����ѯ�����ཻ������ܻ�Ӱ�쵽����ײ
				// ��Ϊ�����ཻ�������ı�ĸ������ͬʱ���������ཻ�����Կ��ܻ���Ӱ��������ײ�е��ཻ״̬
				for (uint32_t i = 0; i < numContacts; i++)
				{
					// j����ǰ����������ײ�еĸ�������
					for (uint32_t j = 0; j < 2; j++)
					{
						// �����ǰ����������ײ������ڸ���ż���
						if (contacts[i].mRigidBodies[j])
						{
							// k�������ཻ�������ı����ײ�еĸ�������
							for (uint32_t k = 0; k < 2; k++)
							{
								// �����ǰ����������ײ�еĸ����뱾���ཻ�������ı����ײ�еĸ�����ͬ
								// ��ô����Ҫ���µ�ǰ�������������ײ���ཻ��Ϣ��������˵�����ཻ���mPenetration
								// ��Ϊ��һ����ײ�е��ཻ�����������ĸ���λ����ת״̬�仯�����ܻ�Ӱ������ײ���ཻ���
								if (contacts[i].mRigidBodies[j] == contacts[idx].mRigidBodies[k])
								{
									// ���㵱ǰ���������ײ���λ�ñ仯���ɸ���������˶��ͽ��˶�һ�����
									deltaPosition = linearChange[k] + Math::Cross(angularChange[k], contacts[i].mRelativeContactPosition[j]);
									// �����λ�ñ仯ͶӰ����ײ���߷����ϣ��õ�����ײ�����ϵı仯�����Ҳ���Ƕ��ཻ��ȵ�Ӱ��
									// Ȼ�������仯����ӵ���ǰ�������������ײ�ϣ�Ҳ���Ǳ����ཻ����Ե�ǰ�����ײ���ཻ�����������Ӱ��
									// ������mPenetration - delta������mPenetration�Ǵ���ײ�еĵ�һ��������ӽ�������
									// ����Ҫ�жϷ��ţ������ǰ��������ڵ�ǰ�����ײ�д��ڵ�һ��λ�þ��Ǽ�������������ڵڶ���λ�þ͸���������Ҫ��
									contacts[i].mPenetration += Math::Dot(deltaPosition, contacts[i].mContactNormal) * (j ? 1.0f : -1.0f);
								}
							}
						}
					}
				}

				// �����굱ǰ��ײ���ཻ��������ı仯�󣬵���������һ��Ȼ�����Ѱ�Ҹ��º��ཻ����������ײ���������ཻ����
				mCurPositionIterations++;
			}
		}

		void ContactResolver::AdjustVelocities(Contact* contacts, uint32_t numContacts, float duration)
		{
			// �����ٶȸı���
			Vector3 linearVelocityChange[2];
			// ���ٶȸı���
			Vector3 angularVelocityChange[2];
			// �������
			float maxSpeed;

			// ��ײ��ıպ��ٶȱ仯��(��������ǰ���������������ѭ��������������Ȼ����ջ��Ƶ������������)
			Vector3 deltaVelocity;
			// ��ǰ�������ײ����
			uint32_t idx;

			mCurVelocityIterations = 0;
			while (mCurVelocityIterations < mMaxVelocityIterations)
			{
				idx = numContacts;
				maxSpeed = mVelocityEpsilon;
				// �ҵ��ٶȸı�������ײ�����ȴ�����Ϊ�ٶȱ仯Խ�������Ч��Ӱ��ҲԽ��
				for (uint32_t i = 0; i < numContacts; i++)
				{
					if (contacts[i].mDesiredDeltaVelocity > maxSpeed)
					{
						maxSpeed = contacts[i].mDesiredDeltaVelocity;
						idx = i;
					}
				}

				// ˵��û����Ҫ������ٶȱ仯�ˣ�ֱ���˳�
				if (idx == numContacts)
					break;

				// ���ѿ��ܴ�������״̬�ĸ���
				contacts[idx].MatchAwakeState();

				// �����ٶȱ仯������ȡ�ٶȱ仯��Ϣ�������ʵ�ʸı�����������ٶ�״̬
				contacts[idx].ResolveVelocityChange(linearVelocityChange, angularVelocityChange);

				// �����ѭ���߼�ͬAdjustPositions
				for (uint32_t i = 0; i < numContacts; i++)
				{
					for (uint32_t j = 0; j < 2; j++)
					{
						if (contacts[i].mRigidBodies[j])
						{
							for (uint32_t k = 0; k < 2; k++)
							{
								if (contacts[i].mRigidBodies[j] == contacts[idx].mRigidBodies[k])
								{
									// ��������ٶȸı����ͽ��ٶȸı�����������ײ��ıպ��ٶȱ仯��
									deltaVelocity = linearVelocityChange[k] + Math::Cross(angularVelocityChange[k], contacts[i].mRelativeContactPosition[j]);
									// ���仯��ת������ײ�ռ䣬Ȼ��ӵ�λ����ײ�ռ�ıպ��ٶ���
									contacts[i].mContactVelocity += Math::Transpose(contacts[i].mContactToWorld) * deltaVelocity * (j ? -1.0f : 1.0f);
									// ���������ٶȱ仯��
									contacts[i].UpdateDesiredDeltaVelocity(duration);
								}
							}
						}
					}
				}

				// ����������һ������Ѱ���ٶȱ仯������ײ���������ٶȱ仯����
				mCurVelocityIterations++;
			}
		}
	}
}