#include "CollisionDetector.h"
#include "CollisionData.h"
#include "CollisionPrimitive.h"
#include "Contact.h"
#include "RigidBody.h"
#include "IntersectionDetector.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		uint32_t CollisionDetector::Detect(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data)
		{
			// ����ʣ����ײ��Ҫ���ż���
			if (data->IsFull()) 
				return 0;

			// ��������ϵ�µ�����
			Vector3 gSphereCenter = sphere.mTransform.GetColumn(3);
			// Box����ϵ�µ�����
			Vector3 lSphereCenter = Math::Inverse(box.mTransform) * Vector4(gSphereCenter, 1.0f);

			// �����ж��Ƿ��ཻ(Ŀ����������ܣ�����û��Ҫ����ȷ���ཻ���)
			if (fabsf(lSphereCenter.x) - sphere.mRadius > box.mHalfSize.x ||
				fabsf(lSphereCenter.y) - sphere.mRadius > box.mHalfSize.y ||
				fabsf(lSphereCenter.z) - sphere.mRadius > box.mHalfSize.z)
			{
				return 0;
			}

			// Box������������ĵ�
			Vector3 closestPoint(0.0f, 0.0f, 0.0f);
			// ��ʱ����
			float distance;

			// ������������
			distance = lSphereCenter.x;
			if (distance >  box.mHalfSize.x) distance =  box.mHalfSize.x;
			if (distance < -box.mHalfSize.x) distance = -box.mHalfSize.x;
			closestPoint.x = distance;

			distance = lSphereCenter.y;
			if (distance >  box.mHalfSize.y) distance =  box.mHalfSize.y;
			if (distance < -box.mHalfSize.y) distance = -box.mHalfSize.y;
			closestPoint.y = distance;

			distance = lSphereCenter.z;
			if (distance >  box.mHalfSize.z) distance =  box.mHalfSize.z;
			if (distance < -box.mHalfSize.z) distance = -box.mHalfSize.z;
			closestPoint.z = distance;

			// �ж��Ƿ��ཻ
			distance = (closestPoint - lSphereCenter).GetMagnitudeSquared();
			if (distance > sphere.mRadius * sphere.mRadius)
				return 0;

			// �任����������ϵ
			closestPoint = box.mTransform * Vector4(closestPoint, 1.0f);

			// ��ǰҪд�����ײ
			Contact* contact = data->mCurContact;
			// ��ײ����
			contact->mContactNormal = (closestPoint - gSphereCenter).GetNormalized();
			// ��ײ��
			contact->mContactPoint = closestPoint;
			// ��ײ���
			contact->mPenetration = sphere.mRadius - sqrtf(distance);
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(box.mRigidBody, sphere.mRigidBody);

			data->AddContacts(1);
			return 1;
		}

		uint32_t CollisionDetector::Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2, CollisionData* data)
		{
			// ����ʣ����ײ��Ҫ���ż���
			if (data->IsFull()) 
				return 0;

			// ������ײ���λ��
			Vector3 pos1 = sphere1.mTransform.GetColumn(3);
			Vector3 pos2 = sphere2.mTransform.GetColumn(3);

			Vector3 centerLine = pos1 - pos2;
			float distance = centerLine.GetMagnitude();

			// ������ײ��ľ���������߰뾶֮������ײ
			if (distance <= 0.0f || distance >= sphere1.mRadius + sphere2.mRadius)
			{
				return 0;
			}

			// ��ǰҪд�����ײ
			Contact* contact = data->mCurContact;
			// ��ײ����
			contact->mContactNormal = centerLine.GetNormalized();
			// ��ײ��(��һ�����������������߽���)
			contact->mContactPoint = pos1 - contact->mContactNormal * sphere1.mRadius;
			// ��ײ���
			contact->mPenetration = (sphere1.mRadius + sphere2.mRadius) - distance;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere1.mRigidBody, sphere2.mRigidBody);

			data->AddContacts(1);
			return 1;
		}

		static const Vector3 boxVertex[8] =
		{
			{ 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f, -1.0f}, { 1.0f, -1.0f, 1.0f}, { 1.0f, -1.0f, -1.0f},
			{-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f}, {-1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f}
		};
		uint32_t CollisionDetector::Detect(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data, bool isHalfSpace)
		{
			// ����ʣ����ײ��Ҫ���ż���
			if (data->IsFull())
				return 0;

			// ����һ���ཻ����
			if (!IntersectionDetector::Detect(box, plane))
				return 0;

			Contact* contact = data->mCurContact;
			uint32_t contactNum = 0;
			for (size_t i = 0; i < 8; i++)
			{
				// ��������ϵ�µ�Box����
				Vector3 vertexPos = box.mTransform * Vector4(boxVertex[i] * box.mHalfSize, 1.0f);
				// ������ƽ�淨�߷�����������ϵԭ��ľ���
				float distance = Math::Dot(vertexPos, plane.mNormal);

				if (distance <= plane.mDistance)
				{
					// ��ײ����ֱ����ƽ�淨��
					contact->mContactNormal = plane.mNormal;
					// ��ײ��(��ƽ����)
					contact->mContactPoint = vertexPos + plane.mNormal * (plane.mDistance - distance);
					// ��ײ���(��ײ�㵽ƽ��ľ���)
					contact->mPenetration = plane.mDistance - distance;
					contact->mRestitution = data->mRestitution;
					contact->mFriction = data->mFriction;
					contact->SetRigidBodies(box.mRigidBody, nullptr);
					
					// ָ�����
					contact++;
					contactNum++;

					// ��ײ�����Ѿ����ˣ�ֱ�ӷ���
					if (contactNum == data->mContactsLeft)
					{
						data->AddContacts(contactNum);
						return contactNum;
					}
				}
			}

			data->AddContacts(contactNum);
			return contactNum;
		}

		uint32_t CollisionDetector::Detect(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data, bool isHalfSpace)
		{
			if (isHalfSpace)
				return DetectSphereAndHalfSpace(sphere, plane, data);
			else
				return DetectSphereAndPlane(sphere, plane, data);
		}

		uint32_t CollisionDetector::DetectSphereAndPlane(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
		{
			// ����ʣ����ײ��Ҫ���ż���
			if (data->IsFull())
				return 0;

			// ��ײ���λ��
			Vector3 pos = sphere.mTransform.GetColumn(3);

			// ��ײ�����ĵ���ײƽ��ľ���
			float distance = Math::Dot(plane.mNormal, pos) - plane.mDistance;

			// ��ײ����������ײƽ��ľ��������ײ��뾶����ײ
			if (abs(distance) >= sphere.mRadius)
				return 0;

			Vector3 normal = plane.mNormal;
			float penetration = -distance;
			// ����Ϊ��˵������ƽ��ı��棬����ȡ��
			if (distance < 0.0f)
			{
				normal *= -1.0f;
				penetration = -penetration;
			}
			penetration += sphere.mRadius;

			// ��ǰҪд�����ײ
			Contact* contact = data->mCurContact;
			// ��ײ����
			contact->mContactNormal = normal;
			// ��ײ��(��ƽ����)
			contact->mContactPoint = pos - plane.mNormal * distance;
			// ��ײ���(��ײ�㵽ƽ��ľ���)
			contact->mPenetration = penetration;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere.mRigidBody, nullptr);

			data->AddContacts(1);
			return 1;
		}

		uint32_t CollisionDetector::DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
		{
			// ����ʣ����ײ��Ҫ���ż���
			if (data->IsFull())
				return 0;

			// ��ײ���λ��
			Vector3 pos = sphere.mTransform.GetColumn(3);

			// ��ײ����ײƽ��ľ���
			float distance = Math::Dot(plane.mNormal, pos) - sphere.mRadius - plane.mDistance;

			// ��ײ������ײƽ��ľ������0����ײ
			if (distance >= 0.0f)
				return 0;

			// ��ǰҪд�����ײ
			Contact* contact = data->mCurContact;
			// ��ײ����
			contact->mContactNormal = plane.mNormal;
			// ��ײ��(��ƽ����)
			contact->mContactPoint = pos - plane.mNormal * (distance + sphere.mRadius);
			// ��ײ���(��ײ�㵽ƽ��ľ���)
			contact->mPenetration = -distance;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere.mRigidBody, nullptr);

			data->AddContacts(1);
			return 1;
		}
	}
}