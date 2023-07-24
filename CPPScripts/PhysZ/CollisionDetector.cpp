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
			// 还有剩余碰撞需要检测才继续
			if (data->mContactsLeft <= 0) 
				return 0;

			// 两个碰撞球的位置
			Vector3 pos1 = sphere1->mTransform.GetColumn(3);
			Vector3 pos2 = sphere2->mTransform.GetColumn(3);

			Vector3 centerLine = pos1 - pos2;
			float distance = centerLine.GetMagnitude();

			// 两个碰撞球的距离大于两者半径之和则不碰撞
			if (distance <= 0.0f || distance >= sphere1->mRadius + sphere2->mRadius)
			{
				return 0;
			}

			// 当前要写入的碰撞
			Contact* contact = data->mCurContact;
			// 碰撞法线
			contact->mContactNormal = centerLine.GetNormalized();
			// 碰撞点
			contact->mContactPoint = pos1 + centerLine * 0.5f;
			// 碰撞深度
			contact->mPenetration = (sphere1->mRadius + sphere2->mRadius) - distance;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere1->mRigidBody, sphere2->mRigidBody);

			data->AddContacts(1);
			return 1;
		}
	}
}