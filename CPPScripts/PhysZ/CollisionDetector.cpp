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

		// 用于辅助Box和Box的碰撞检测
		// 基于分离轴算法，返回在某个轴上是否重叠，并更新最小重叠大小和对应的分离轴索引
		static inline bool TryAxis(
			const CollisionBox& box1, 
			const CollisionBox& box2, 
			Vector3 axis, 
			const Vector3& centerLine, 
			uint32_t index, 
			float& smallestPenetration, 
			uint32_t& smallestCase)
		{
			// 如果当前检测的分离轴的模长接近0，说明是由两个Box的两条轴(自己局部坐标系的XYZ轴之一)叉乘出来的分离轴
			// 并且这两条轴几乎平行，跳过这种分离轴不检测
			if (axis.GetMagnitudeSquared() < 0.0001f)
				return true;

			axis.Normalize();

			// 计算两个Box在axis上的重叠长度
			float penetration = IntersectionDetector::GetPenetrationOnAxis(box1, box2, axis, centerLine);

			// 如果重叠长度小于0，说明两个Box在axis上没有重叠，不相交
			if (penetration < 0.0f)
				return false;

			// 记录最小重叠长度和分离轴索引
			if (penetration < smallestPenetration)
			{
				smallestPenetration = penetration;
				smallestCase = index;
			}

			return true;
		}

		// 用于辅助Box和Box的碰撞检测
		// 当Box2的一个顶点在Box1的内部时，添加一个点面碰撞
		static void FillPointFaceContact(
			const CollisionBox& box1,
			const CollisionBox& box2,
			const Vector3& centerLine,
			CollisionData* data,
			uint32_t axisIdx,
			float penetration)
		{
			// 碰撞法线为Box1对应面的法线
			Vector3 normal = box1.mTransform.GetColumn(axisIdx);
			// 这里是让碰撞法线方向始终是从Box2到Box1的
			if (Math::Dot(normal, centerLine) > 0.0f)
				normal = normal * -1.0f;

			// 计算具体是Box2的哪个顶点发生了碰撞
			Vector3 vertex = box2.mHalfSize;
			// 如果Box2在Box1的左侧，那么Box2的顶点的X坐标为Box2的半长
			if (Math::Dot(box2.mTransform.GetColumn(0), normal) < 0.0f)
				vertex.x = -vertex.x;
			// 如果Box2在Box1的下侧，那么Box2的顶点的Y坐标为Box2的半宽
			if (Math::Dot(box2.mTransform.GetColumn(1), normal) < 0.0f)
				vertex.y = -vertex.y;
			// 如果Box2在Box1的后侧，那么Box2的顶点的Z坐标为Box2的半高
			if (Math::Dot(box2.mTransform.GetColumn(2), normal) < 0.0f)
				vertex.z = -vertex.z;

			// 当前要写入的碰撞
			Contact* contact = data->mCurContact;
			// 碰撞法线
			contact->mContactNormal = normal;
			// 碰撞点(Box2顶点变换到世界坐标系)
			contact->mContactPoint = box2.mTransform * vertex;
			// 碰撞深度
			contact->mPenetration = penetration;
			contact->mRestitution = data->mRestitution;
			contact->mFriction = data->mFriction;
			contact->SetRigidBodies(box1.mRigidBody, box2.mRigidBody);
		}

#define CHECK_OVERLAP(axis, index) if (!TryAxis(box1, box2, (axis), centerLine, (index), penetration, axisIdx)) return 0;

		uint32_t CollisionDetector::Detect(const CollisionBox& box1, const CollisionBox& box2, CollisionData* data)
		{
			Vector3 centerLine = box2.mTransform.GetColumn(3) - box1.mTransform.GetColumn(3);

			// 用于记录最小重叠长度和对应的分离轴索引
			float penetration = FLT_MAX;
			uint32_t axisIdx = 0xFF;

			// 分别检测两个Box的三条轴
			CHECK_OVERLAP(box1.mTransform.GetColumn(0), 0);
			CHECK_OVERLAP(box1.mTransform.GetColumn(1), 1);
			CHECK_OVERLAP(box1.mTransform.GetColumn(2), 2);

			CHECK_OVERLAP(box2.mTransform.GetColumn(0), 3);
			CHECK_OVERLAP(box2.mTransform.GetColumn(1), 4);
			CHECK_OVERLAP(box2.mTransform.GetColumn(2), 5);

			// 计算两个Box的三条轴的叉乘轴
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

			// Box2的一个顶点碰撞到Box1的一个面
			if (axisIdx < 3)
			{
				FillPointFaceContact(box1, box2, centerLine, data, axisIdx, penetration);
				data->AddContacts(1);
				return 1;
			}
			// Box1的一个顶点碰撞到Box2的一个面
			else if (axisIdx < 6)
			{
				// 交换Box1和Box2，centerLine取反
				FillPointFaceContact(box2, box1, centerLine * -1.0f, data, axisIdx - 3, penetration);
				data->AddContacts(1);
				return 1;
			}
			// Box1和Box2的一个边碰撞到对方的一个边
			else
			{
				// 获取分离轴
				axisIdx -= 6;
				uint32_t box1AxisIdx = axisIdx / 3;
				uint32_t box2AxisIdx = axisIdx % 3;
				Vector3 axis1 = box1.mTransform.GetColumn(box1AxisIdx);
				Vector3 axis2 = box2.mTransform.GetColumn(box2AxisIdx);
				Vector3 axis = Math::Cross(axis1, axis2);
				axis.Normalize();

				// 确保分离轴从Box1指向Box2
				if (Math::Dot(axis, centerLine) > 0.0f)
					axis = axis * -1.0f;

				// Box1和Box2上各自有4条边和分离轴垂直，需要定位相交的2条边
				// 这里先计算各自4条边的中点里，离另一个Box最近的
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

				// 把这两个中点转换到世界坐标系下
				midPoint1 = box1.mTransform * Vector4(midPoint1, 1.0f);
				midPoint2 = box2.mTransform * Vector4(midPoint2, 1.0f);

				// 获取两条线段的交点
				Vector3 contactPoint;
				IntersectionDetector::DetectLineSegmentContact(
					midPoint1, axis1, box1.mHalfSize[box1AxisIdx], 
					midPoint2, axis2, box2.mHalfSize[box2AxisIdx],
					contactPoint, axisIdx > 2);

				// 当前要写入的碰撞
				Contact* contact = data->mCurContact;
				// 碰撞法线
				contact->mContactNormal = axis;
				// 碰撞点
				contact->mContactPoint = contactPoint;
				// 碰撞深度
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