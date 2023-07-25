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

		bool IntersectionDetector::IsOverlapOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine)
		{
			float projectedLength1 = box1.GetHalfProjectedLength(axis);
			float projectedLength2 = box2.GetHalfProjectedLength(axis);

			float distance = fabsf(Math::Dot(axis, centerLine));

			return distance < projectedLength1 + projectedLength2;
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
			float distance = Math::Dot(plane.mNormal, box.mTransform.GetColumn(3)) - projectedLength;
			return distance <= plane.mDistance;
		}

		bool IntersectionDetector::DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane)
		{
			float distance = Math::Dot(plane.mNormal, sphere.mTransform.GetColumn(3)) - sphere.mRadius;
			return distance <= plane.mDistance;
		}
	}
}