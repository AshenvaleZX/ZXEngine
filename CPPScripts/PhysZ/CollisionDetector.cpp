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
		uint32_t CollisionDetector::Detect(const CollisionPrimitive* p1, const CollisionPrimitive* p2, CollisionData* data)
		{
			if (p1 == nullptr || p2 == nullptr || data == nullptr)
				return 0;

			auto type1 = p1->GetType();
			auto type2 = p2->GetType();

			if (type1 == ColliderType::Box && type2 == ColliderType::Box)
			{
				auto c1 = static_cast<const CollisionBox*>(p1);
				auto c2 = static_cast<const CollisionBox*>(p2);
				return Detect(*c1, *c2, data);
			}
			else if (type1 == ColliderType::Box && type2 == ColliderType::Sphere)
			{
				auto c1 = static_cast<const CollisionBox*>(p1);
				auto c2 = static_cast<const CollisionSphere*>(p2);
				return Detect(*c1, *c2, data);
			}
			else if (type1 == ColliderType::Sphere && type2 == ColliderType::Box)
			{
				auto c1 = static_cast<const CollisionSphere*>(p1);
				auto c2 = static_cast<const CollisionBox*>(p2);
				return Detect(*c2, *c1, data);
			}
			else if (type1 == ColliderType::Sphere && type2 == ColliderType::Sphere)
			{
				auto c1 = static_cast<const CollisionSphere*>(p1);
				auto c2 = static_cast<const CollisionSphere*>(p2);
				return Detect(*c1, *c2, data);
			}
			else if (type1 == ColliderType::Box && type2 == ColliderType::Plane)
			{
				auto c1 = static_cast<const CollisionBox*>(p1);
				auto c2 = static_cast<const CollisionPlane*>(p2);
				return Detect(*c1, *c2, data);
			}
			else if (type1 == ColliderType::Plane && type2 == ColliderType::Box)
			{
				auto c1 = static_cast<const CollisionPlane*>(p1);
				auto c2 = static_cast<const CollisionBox*>(p2);
				return Detect(*c2, *c1, data);
			}
			else if (type1 == ColliderType::Sphere && type2 == ColliderType::Plane)
			{
				auto c1 = static_cast<const CollisionSphere*>(p1);
				auto c2 = static_cast<const CollisionPlane*>(p2);
				return Detect(*c1, *c2, data);
			}
			else if (type1 == ColliderType::Plane && type2 == ColliderType::Sphere)
			{
				auto c1 = static_cast<const CollisionPlane*>(p1);
				auto c2 = static_cast<const CollisionSphere*>(p2);
				return Detect(*c2, *c1, data);
			}

			return 0;
		}

		// ���ڸ���Box��Box����ײ���
		// ���ڷ������㷨��������ĳ�������Ƿ��ص�����������С�ص���С�Ͷ�Ӧ�ķ���������
		static inline bool TryAxis(
			const CollisionBox& box1, 
			const CollisionBox& box2, 
			Vector3 axis, 
			const Vector3& centerLine, 
			uint32_t index, 
			float& smallestPenetration, 
			uint32_t& smallestCase)
		{
			// �����ǰ���ķ������ģ���ӽ�0��˵����������Box��������(�Լ��ֲ�����ϵ��XYZ��֮һ)��˳����ķ�����
			// �����������Ἰ��ƽ�У��������ַ����᲻���
			if (axis.GetMagnitudeSquared() < 0.0001f)
				return true;

			axis.Normalize();

			// ��������Box��axis�ϵ��ص�����
			float penetration = IntersectionDetector::GetPenetrationOnAxis(box1, box2, axis, centerLine);

			// ����ص�����С��0��˵������Box��axis��û���ص������ཻ
			if (penetration < 0.0f)
				return false;

			// ��¼��С�ص����Ⱥͷ���������
			if (penetration < smallestPenetration)
			{
				smallestPenetration = penetration;
				smallestCase = index;
			}

			return true;
		}

		// ���ڸ���Box��Box����ײ���
		// ��Box2��һ��������Box1���ڲ�ʱ�����һ��������ײ
		static void FillPointFaceContact(
			const CollisionBox& box1,
			const CollisionBox& box2,
			const Vector3& centerLine,
			CollisionData* data,
			uint32_t axisIdx,
			float penetration)
		{
			// ��ײ����ΪBox1��Ӧ��ķ���
			Vector3 normal = box1.mTransform.GetColumn(axisIdx);
			// ����������ײ���߷���ʼ���Ǵ�Box2��Box1��
			if (Math::Dot(normal, centerLine) > 0.0f)
				normal = normal * -1.0f;

			// ���������Box2���ĸ����㷢������ײ
			Vector3 vertex = box2.mHalfSize;
			// ���Box2��Box1����࣬��ôBox2�Ķ����X����ΪBox2�İ볤
			if (Math::Dot(box2.mTransform.GetColumn(0), normal) < 0.0f)
				vertex.x = -vertex.x;
			// ���Box2��Box1���²࣬��ôBox2�Ķ����Y����ΪBox2�İ��
			if (Math::Dot(box2.mTransform.GetColumn(1), normal) < 0.0f)
				vertex.y = -vertex.y;
			// ���Box2��Box1�ĺ�࣬��ôBox2�Ķ����Z����ΪBox2�İ��
			if (Math::Dot(box2.mTransform.GetColumn(2), normal) < 0.0f)
				vertex.z = -vertex.z;

			// ��ǰҪд�����ײ
			Contact* contact = data->mCurContact;
			// ��ײ����
			contact->mContactNormal = normal;
			// ��ײ��(Box2����任����������ϵ)
			contact->mContactPoint = box2.mTransform * vertex;
			// ��ײ���
			contact->mPenetration = penetration;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(box1.mRigidBody, box2.mRigidBody);
		}

#define CHECK_OVERLAP(axis, index) if (!TryAxis(box1, box2, (axis), centerLine, (index), penetration, axisIdx)) return 0;

		uint32_t CollisionDetector::Detect(const CollisionBox& box1, const CollisionBox& box2, CollisionData* data)
		{
			Vector3 centerLine = box2.mTransform.GetColumn(3) - box1.mTransform.GetColumn(3);

			// ���ڼ�¼��С�ص����ȺͶ�Ӧ�ķ���������
			float penetration = FLT_MAX;
			uint32_t axisIdx = 0xFF;

			// �ֱ�������Box��������
			CHECK_OVERLAP(box1.mTransform.GetColumn(0), 0);
			CHECK_OVERLAP(box1.mTransform.GetColumn(1), 1);
			CHECK_OVERLAP(box1.mTransform.GetColumn(2), 2);

			CHECK_OVERLAP(box2.mTransform.GetColumn(0), 3);
			CHECK_OVERLAP(box2.mTransform.GetColumn(1), 4);
			CHECK_OVERLAP(box2.mTransform.GetColumn(2), 5);

			// ��������Box��������Ĳ����
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(0)), 6);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(1)), 7);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(2)), 8);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(0)), 9);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(1)), 10);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(2)), 11);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(0)), 12);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(1)), 13);
			CHECK_OVERLAP(Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(2)), 14);

			if (axisIdx == 0xFF)
			{
				Debug::LogError("CollisionDetector::Detect: Invalid axis index!");
				return 0;
			}

			// Box2��һ��������ײ��Box1��һ����
			if (axisIdx < 3)
			{
				FillPointFaceContact(box1, box2, centerLine, data, axisIdx, penetration);
				data->AddContacts(1);
				return 1;
			}
			// Box1��һ��������ײ��Box2��һ����
			else if (axisIdx < 6)
			{
				// ����Box1��Box2��centerLineȡ��
				FillPointFaceContact(box2, box1, centerLine * -1.0f, data, axisIdx - 3, penetration);
				data->AddContacts(1);
				return 1;
			}
			// Box1��Box2��һ������ײ���Է���һ����
			else
			{
				// ��ȡ������
				axisIdx -= 6;
				uint32_t box1AxisIdx = axisIdx / 3;
				uint32_t box2AxisIdx = axisIdx % 3;
				Vector3 axis1 = box1.mTransform.GetColumn(box1AxisIdx);
				Vector3 axis2 = box2.mTransform.GetColumn(box2AxisIdx);
				Vector3 axis = Math::Cross(axis1, axis2);
				axis.Normalize();

				// ȷ���������Box1ָ��Box2
				if (Math::Dot(axis, centerLine) > 0.0f)
					axis = axis * -1.0f;

				// Box1��Box2�ϸ�����4���ߺͷ����ᴹֱ����Ҫ��λ�ཻ��2����
				// �����ȼ������4���ߵ��е������һ��Box�����
				Vector3 midPoint1 = box1.mHalfSize;
				Vector3 midPoint2 = box2.mHalfSize;
				for (uint32_t i = 0; i < 3; ++i)
				{
					if (i == box1AxisIdx)
						midPoint1[i] = 0.0f;
					else if (Math::Dot(box1.mTransform.GetColumn(i), axis) > 0.0f)
						midPoint1[i] = -midPoint1[i];

					if (i == box2AxisIdx)
						midPoint2[i] = 0.0f;
					else if (Math::Dot(box2.mTransform.GetColumn(i), axis) < 0.0f)
						midPoint2[i] = -midPoint2[i];
				}

				// ���������е�ת������������ϵ��
				midPoint1 = box1.mTransform * Vector4(midPoint1, 1.0f);
				midPoint2 = box2.mTransform * Vector4(midPoint2, 1.0f);

				// ��ȡ�����߶εĽ���
				Vector3 contactPoint;
				IntersectionDetector::DetectLineSegmentContact(
					midPoint1, axis1, box1.mHalfSize[box1AxisIdx], 
					midPoint2, axis2, box2.mHalfSize[box2AxisIdx],
					contactPoint, axisIdx > 2);

				// ��ǰҪд�����ײ
				Contact* contact = data->mCurContact;
				// ��ײ����
				contact->mContactNormal = axis;
				// ��ײ��
				contact->mContactPoint = contactPoint;
				// ��ײ���
				contact->mPenetration = penetration;
				contact->mRestitution = data->mRestitution;
				contact->mFriction = data->mFriction;
				contact->SetRigidBodies(box1.mRigidBody, box2.mRigidBody);

				data->AddContacts(1);
				return 1;
			}
		}

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