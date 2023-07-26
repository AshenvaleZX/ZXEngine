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
			// 还有剩余碰撞需要检测才继续
			if (data->IsFull()) 
				return 0;

			// 世界坐标系下的球心
			Vector3 gSphereCenter = sphere.mTransform.GetColumn(3);
			// Box坐标系下的球心
			Vector3 lSphereCenter = Math::Inverse(box.mTransform) * Vector4(gSphereCenter, 1.0f);

			// 粗略判断是否相交(目的是提高性能，所以没必要做精确的相交检测)
			if (fabsf(lSphereCenter.x) - sphere.mRadius > box.mHalfSize.x ||
				fabsf(lSphereCenter.y) - sphere.mRadius > box.mHalfSize.y ||
				fabsf(lSphereCenter.z) - sphere.mRadius > box.mHalfSize.z)
			{
				return 0;
			}

			// Box上离球心最近的点
			Vector3 closestPoint(0.0f, 0.0f, 0.0f);
			// 临时变量
			float distance;

			// 分轴计算最近点
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

			// 判断是否相交
			distance = (closestPoint - lSphereCenter).GetMagnitudeSquared();
			if (distance > sphere.mRadius * sphere.mRadius)
				return 0;

			// 变换到世界坐标系
			closestPoint = box.mTransform * Vector4(closestPoint, 1.0f);

			// 当前要写入的碰撞
			Contact* contact = data->mCurContact;
			// 碰撞法线
			contact->mContactNormal = (closestPoint - gSphereCenter).GetNormalized();
			// 碰撞点
			contact->mContactPoint = closestPoint;
			// 碰撞深度
			contact->mPenetration = sphere.mRadius - sqrtf(distance);
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(box.mRigidBody, sphere.mRigidBody);

			data->AddContacts(1);
			return 1;
		}

		uint32_t CollisionDetector::Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2, CollisionData* data)
		{
			// 还有剩余碰撞需要检测才继续
			if (data->IsFull()) 
				return 0;

			// 两个碰撞球的位置
			Vector3 pos1 = sphere1.mTransform.GetColumn(3);
			Vector3 pos2 = sphere2.mTransform.GetColumn(3);

			Vector3 centerLine = pos1 - pos2;
			float distance = centerLine.GetMagnitude();

			// 两个碰撞球的距离大于两者半径之和则不碰撞
			if (distance <= 0.0f || distance >= sphere1.mRadius + sphere2.mRadius)
			{
				return 0;
			}

			// 当前要写入的碰撞
			Contact* contact = data->mCurContact;
			// 碰撞法线
			contact->mContactNormal = centerLine.GetNormalized();
			// 碰撞点(第一个球的球面和球心连线交点)
			contact->mContactPoint = pos1 - contact->mContactNormal * sphere1.mRadius;
			// 碰撞深度
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
			// 还有剩余碰撞需要检测才继续
			if (data->IsFull())
				return 0;

			// 先做一个相交测试
			if (!IntersectionDetector::Detect(box, plane))
				return 0;

			Contact* contact = data->mCurContact;
			uint32_t contactNum = 0;
			for (size_t i = 0; i < 8; i++)
			{
				// 世界坐标系下的Box顶点
				Vector3 vertexPos = box.mTransform * Vector4(boxVertex[i] * box.mHalfSize, 1.0f);
				// 顶点沿平面法线方向到世界坐标系原点的距离
				float distance = Math::Dot(vertexPos, plane.mNormal);

				if (distance <= plane.mDistance)
				{
					// 碰撞法线直接用平面法线
					contact->mContactNormal = plane.mNormal;
					// 碰撞点(在平面上)
					contact->mContactPoint = vertexPos + plane.mNormal * (plane.mDistance - distance);
					// 碰撞深度(碰撞点到平面的距离)
					contact->mPenetration = plane.mDistance - distance;
					contact->mRestitution = data->mRestitution;
					contact->mFriction = data->mFriction;
					contact->SetRigidBodies(box.mRigidBody, nullptr);
					
					// 指针后移
					contact++;
					contactNum++;

					// 碰撞数据已经满了，直接返回
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
			// 还有剩余碰撞需要检测才继续
			if (data->IsFull())
				return 0;

			// 碰撞球的位置
			Vector3 pos = sphere.mTransform.GetColumn(3);

			// 碰撞球球心到碰撞平面的距离
			float distance = Math::Dot(plane.mNormal, pos) - plane.mDistance;

			// 碰撞球球心与碰撞平面的距离大于碰撞球半径则不碰撞
			if (abs(distance) >= sphere.mRadius)
				return 0;

			Vector3 normal = plane.mNormal;
			float penetration = -distance;
			// 距离为负说明球在平面的背面，数据取反
			if (distance < 0.0f)
			{
				normal *= -1.0f;
				penetration = -penetration;
			}
			penetration += sphere.mRadius;

			// 当前要写入的碰撞
			Contact* contact = data->mCurContact;
			// 碰撞法线
			contact->mContactNormal = normal;
			// 碰撞点(在平面上)
			contact->mContactPoint = pos - plane.mNormal * distance;
			// 碰撞深度(碰撞点到平面的距离)
			contact->mPenetration = penetration;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere.mRigidBody, nullptr);

			data->AddContacts(1);
			return 1;
		}

		uint32_t CollisionDetector::DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data)
		{
			// 还有剩余碰撞需要检测才继续
			if (data->IsFull())
				return 0;

			// 碰撞球的位置
			Vector3 pos = sphere.mTransform.GetColumn(3);

			// 碰撞球到碰撞平面的距离
			float distance = Math::Dot(plane.mNormal, pos) - sphere.mRadius - plane.mDistance;

			// 碰撞球与碰撞平面的距离大于0则不碰撞
			if (distance >= 0.0f)
				return 0;

			// 当前要写入的碰撞
			Contact* contact = data->mCurContact;
			// 碰撞法线
			contact->mContactNormal = plane.mNormal;
			// 碰撞点(在平面上)
			contact->mContactPoint = pos - plane.mNormal * (distance + sphere.mRadius);
			// 碰撞深度(碰撞点到平面的距离)
			contact->mPenetration = -distance;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(sphere.mRigidBody, nullptr);

			data->AddContacts(1);
			return 1;
		}
	}
}