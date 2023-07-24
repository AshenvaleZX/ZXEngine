#include "CollisionDetector.h"
#include "CollisionData.h"
#include "CollisionPrimitive.h"
#include "Contact.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		uint32_t CollisionDetector::Detect(CollisionSphere* sphere1, CollisionSphere* sphere2, CollisionData* data)
		{
			// ����ʣ����ײ��Ҫ���ż���
			if (data->mContactsLeft <= 0) 
				return 0;

			// ������ײ���λ��
			Vector3 pos1 = sphere1->mTransform.GetColumn(3);
			Vector3 pos2 = sphere2->mTransform.GetColumn(3);

			Vector3 centerLine = pos1 - pos2;
			float distance = centerLine.GetMagnitude();

			// ������ײ��ľ���������߰뾶֮������ײ
			if (distance <= 0.0f || distance >= sphere1->mRadius + sphere2->mRadius)
			{
				return 0;
			}

			// ��ǰҪд�����ײ
			Contact* contact = data->mCurContact;
			// ��ײ����
			contact->mContactNormal = centerLine.GetNormalized();
			// ��ײ��
			contact->mContactPoint = pos1 + centerLine * 0.5f;
			// ��ײ���
			contact->mPenetration = (sphere1->mRadius + sphere2->mRadius) - distance;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere1->mRigidBody, sphere2->mRigidBody);

			data->AddContacts(1);
			return 1;
		}
	}
}