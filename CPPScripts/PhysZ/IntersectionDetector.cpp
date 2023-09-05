#include "IntersectionDetector.h"
#include "CollisionPrimitive.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		bool IntersectionDetector::Detect(const CollisionBox& box1, const CollisionBox& box2)
		{
			Vector3 centerLine = box1.mTransform.GetColumn(3) - box2.mTransform.GetColumn(3);

			// 分离轴定理(若两个凸面体不相交，则一定至少存在一根轴，使这两个凸面体投影到这根轴上后没有重叠)
			return 
				IsOverlapOnAxis(box1, box2, box1.mTransform.GetColumn(0), centerLine) &&
				IsOverlapOnAxis(box1, box2, box1.mTransform.GetColumn(1), centerLine) &&
				IsOverlapOnAxis(box1, box2, box1.mTransform.GetColumn(2), centerLine) &&

				IsOverlapOnAxis(box1, box2, box2.mTransform.GetColumn(0), centerLine) &&
				IsOverlapOnAxis(box1, box2, box2.mTransform.GetColumn(1), centerLine) &&
				IsOverlapOnAxis(box1, box2, box2.mTransform.GetColumn(2), centerLine) &&

				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(0)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(1)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(0), box2.mTransform.GetColumn(2)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(0)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(1)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(1), box2.mTransform.GetColumn(2)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(0)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(1)), centerLine) &&
				IsOverlapOnAxis(box1, box2, Math::Cross(box1.mTransform.GetColumn(2), box2.mTransform.GetColumn(2)), centerLine);
		}

		bool IntersectionDetector::Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2)
		{
			Vector3 centerLine = sphere1.mTransform.GetColumn(3) - sphere2.mTransform.GetColumn(3);
			return centerLine.GetMagnitudeSquared() < (sphere1.mRadius + sphere2.mRadius) * (sphere1.mRadius + sphere2.mRadius);
		}

		bool IntersectionDetector::Detect(const CollisionBox& box, const CollisionPlane& plane)
		{
			return DetectBoxAndHalfSpace(box, plane);
		}

		bool IntersectionDetector::Detect(const CollisionSphere& sphere, const CollisionPlane& plane)
		{
			return DetectSphereAndHalfSpace(sphere, plane);
		}

		bool IntersectionDetector::DetectBoxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane)
		{
			// 计算Box在平面法线上的投影长度
			float projectedLength = box.GetHalfProjectedLength(plane.mNormal);
			// Box到平面的距离
			float distance = Math::Dot(plane.mNormal, Vector3(box.mTransform.GetColumn(3))) - projectedLength;
			return distance <= plane.mDistance;
		}

		bool IntersectionDetector::DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane)
		{
			float distance = Math::Dot(plane.mNormal, Vector3(sphere.mTransform.GetColumn(3))) - sphere.mRadius;
			return distance <= plane.mDistance;
		}

		bool IntersectionDetector::IsOverlapOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine)
		{
			float projectedLength1 = box1.GetHalfProjectedLength(axis);
			float projectedLength2 = box2.GetHalfProjectedLength(axis);

			float distance = fabsf(Math::Dot(axis, centerLine));

			return distance < projectedLength1 + projectedLength2;
		}

		float IntersectionDetector::GetPenetrationOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine)
		{
			float projectedLength1 = box1.GetHalfProjectedLength(axis);
			float projectedLength2 = box2.GetHalfProjectedLength(axis);

			float distance = fabsf(Math::Dot(axis, centerLine));

			return projectedLength1 + projectedLength2 - distance;
		}

		bool IntersectionDetector::DetectLineSegmentContact(
			const Vector3& midPoint1, const Vector3& dir1, float halfLength1,
			const Vector3& midPoint2, const Vector3& dir2, float halfLength2,
			Vector3& contactPoint, bool useOne)
		{
			// 两条线段方向长度的平方
			float squaredLength1 = dir1.GetMagnitudeSquared();
			float squaredLength2 = dir2.GetMagnitudeSquared();
			// 两条线段方向的点积
			float dot_d1_d2 = Math::Dot(dir1, dir2);

			float denominator = squaredLength1 * squaredLength2 - dot_d1_d2 * dot_d1_d2;

			// 如果两条线段平行，那么dot_d1_d2的值就应该是正(夹角0)或负(夹角180)dir1长度*dir2长度
			// 那么(squaredLength1 * squaredLength2)和(dot_d1_d2 * dot_d1_d2)都等价于((dir1长度*dir2长度)^2)
			// 所以如果denominator的值很小，就认为两条线段平行，返回无交叉
			if (fabsf(denominator) < 0.0001f)
			{
				contactPoint = useOne ? midPoint1 : midPoint2;
				return false;
			}

			// 点2到点1的向量
			Vector3 p2top1 = midPoint1 - midPoint2;
			// 点2到点1的向量和两条线段方向的点积
			float dot_2to1_d1 = Math::Dot(p2top1, dir1);
			float dot_2to1_d2 = Math::Dot(p2top1, dir2);

			// 交点到线段1中点的距离
			float distance1 = (dot_d1_d2 * dot_2to1_d2 - squaredLength2 * dot_2to1_d1) / denominator;
			// 交点到线段2中点的距离
			float distance2 = (squaredLength1 * dot_2to1_d2 - dot_d1_d2 * dot_2to1_d1) / denominator;

			// 如果交点没有同时在两条线段上则不相交
			if (distance1 > halfLength1 || distance1 < -halfLength1 || distance2 > halfLength2 || distance2 < -halfLength2)
			{
				contactPoint = useOne ? midPoint1 : midPoint2;
				return false;
			}
			else
			{
				// 通过线段1计算交点
				Vector3 contactPos1 = midPoint1 + dir1 * distance1;
				// 通过线段2计算交点
				Vector3 contactPos2 = midPoint2 + dir2 * distance2;
				// 两个交点的平均值(理想情况下这两个点应该是完全重合的，但是实际运算中基本不可能，所以取平均)
				contactPoint = (contactPos1 + contactPos2) * 0.5f;
				return true;
			}
		}
	}
}