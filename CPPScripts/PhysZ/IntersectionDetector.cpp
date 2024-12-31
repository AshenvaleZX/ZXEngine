#include "IntersectionDetector.h"
#include "CollisionPrimitive.h"
#include "Ray.h"
#include "../ZMesh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		bool IntersectionDetector::Detect(const Ray& ray, const CollisionBox& box)
		{
			RayHitInfo hit;
			return Detect(ray, box, hit);
		}

		bool IntersectionDetector::Detect(const Ray& ray, const CollisionBox& box, RayHitInfo& hit)
		{
			// 将射线转换到Box的局部空间
			Matrix4 transform = Math::Inverse(box.mTransform);
			Ray localRay = Ray(transform * ray.mOrigin.ToPosVec4(), transform * ray.mDirection.ToDirVec4());
			return Detect(localRay, box.mHalfSize, hit);
		}

		bool IntersectionDetector::Detect(const Ray& localRay, const Vector3& boxHalfSize, RayHitInfo& hit)
		{
			// 射线距离的最小最大值
			float tMin = 0.0f;
			float tMax = FLT_MAX;

			// 射线方向的倒数
			Vector3 inverseDirection = Vector3(1.0f / localRay.mDirection.x, 1.0f / localRay.mDirection.y, 1.0f / localRay.mDirection.z);

			// Box的最小最大值
			Vector3 bMin = -boxHalfSize;
			Vector3 bMax =  boxHalfSize;

			for (int i = 0; i < 3; i++)
			{
				if (fabsf(localRay.mDirection[i]) < 0.0001f)
				{
					// 与Box的某个面平行，且起点不在Box内，则不相交
					if (localRay.mOrigin[i] < bMin[i] || localRay.mOrigin[i] > bMax[i])
					{
						return false;
					}
				}
				else
				{
					// pBox - pRayOrigin = t * pRayDirection
					float t1 = (bMin[i] - localRay.mOrigin[i]) * inverseDirection[i];
					float t2 = (bMax[i] - localRay.mOrigin[i]) * inverseDirection[i];

					if (t1 > t2)
					{
						float temp = t1;
						t1 = t2;
						t2 = temp;
					}

					tMin = t1 > tMin ? t1 : tMin;
					tMax = t2 < tMax ? t2 : tMax;

					if (tMin > tMax)
					{
						return false;
					}
				}
			}

			hit.distance = tMin;

			return true;
		}

		bool IntersectionDetector::Detect(const Ray& ray, const CollisionPlane& plane)
		{
			Matrix4 iTrans = Math::Inverse(plane.mTransform);
			// 变化射线的方向向量不同于变化法线，直接用和变化顶点相同的矩阵即可
			Ray localRay = Ray(iTrans * ray.mOrigin.ToPosVec4(), iTrans * ray.mDirection.ToDirVec4());

			// 射线起点在平面的哪一边
			float pSide = Math::Dot(localRay.mOrigin, plane.mLocalNormal);

			// 射线方向和平面法线是否同向
			float rDotN = Math::Dot(localRay.mDirection, plane.mLocalNormal);

			if ((pSide > 0.0f && rDotN > 0.0f) || (pSide < 0.0f && rDotN < 0.0f))
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		bool IntersectionDetector::Detect(const Ray& ray, const CollisionSphere& sphere)
		{
			Vector3 m = ray.mOrigin - sphere.mTransform.GetColumn(3);
			float b = Math::Dot(m, ray.mDirection);
			float c = Math::Dot(m, m) - sphere.mRadius * sphere.mRadius;

			// 射线起点在球外且方向与到球心相反
			if (c > 0.0f && b > 0.0f)
			{
				return false;
			}

			// R(t) = ray.mOrigin + t * ray.mDirection
			// (x - Sphere.x)^2 + (y - Sphere.y)^2 + (z - Sphere.z)^2 = r^2
			// (ray.mOrigin + t * ray.mDirection - Sphere)^2 = r^2
			// (t * ray.mDirection + ray.mOrigin - Sphere)^2 = r^2
			// (t * ray.mDirection + m)^2 = r^2
			// t^2 * ray.mDirection^2 + 2 * t * ray.mDirection * m + m^2 = r^2
			// (ray.mDirection^2) * t^2 + (2 * b) * t + c = 0
			// ray.mDirection^2 = 1
			// t^2 + (2 * b) * t + c = 0

			// y = ax^2 + bx + c
			// 一元二次方程判别式: b^2 - 4ac
			// (2 * b)^2 - 4 * c
			// b^2 - c

			float discr = b * b - c;
			if (discr < 0.0f)
			{
				return false;
			}

			return true;
		}

		bool IntersectionDetector::Detect(const Ray& ray, const CollisionCircle2D& circle)
		{
			RayHitInfo hit;
			return Detect(ray, circle, hit);
		}

		bool IntersectionDetector::Detect(const Ray& ray, const CollisionCircle2D& circle, RayHitInfo& hit)
		{
			Matrix4 iTrans = Math::Inverse(circle.mTransform);
			// 变化射线的方向向量不同于变化法线，直接用和变化顶点相同的矩阵即可
			Ray localRay = Ray(iTrans * ray.mOrigin.ToPosVec4(), iTrans * ray.mDirection.ToDirVec4());

			// 射线起点在平面的哪一边
			float pSide = Math::Dot(localRay.mOrigin, circle.mLocalNormal);

			// 射线方向和平面法线是否同向
			float rDotN = Math::Dot(localRay.mDirection, circle.mLocalNormal);

			if ((pSide > 0.0f && rDotN > 0.0f) || (pSide < 0.0f && rDotN < 0.0f))
			{
				return false;
			}

			hit.distance = -pSide / rDotN;

			Vector3 pHit = localRay.mOrigin + hit.distance * localRay.mDirection;

			return pHit.GetMagnitudeSquared() <= circle.mRadius * circle.mRadius;
		}

		// Moller-Trumbore intersection algorithm
		// "Fast, Minimum Storage Ray-Triangle Intersection", Journal of Graphics Tools, vol. 2, no. 1, pp 21-28, 1997.
		bool IntersectionDetector::Detect(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& t)
		{
			Vector3 e1 = v1 - v0;
			Vector3 e2 = v2 - v0;
			Vector3 p = Math::Cross(ray.mDirection, e2);
			float det = Math::Dot(e1, p);

			// 平行
			if (det > -0.0001f && det < 0.0001f)
			{
				return false;
			}

			float f = 1.0f / det;
			Vector3 s = ray.mOrigin - v0;
			float u = f * Math::Dot(s, p);

			if (u < 0.0f || u > 1.0f)
			{
				return false;
			}

			Vector3 q = Math::Cross(s, e1);
			float v = f * Math::Dot(ray.mDirection, q);

			if (v < 0.0f || u + v > 1.0f)
			{
				return false;
			}

			t = f * Math::Dot(e2, q);

			if (t > 0.0001f)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		bool IntersectionDetector::Detect(const Ray& ray, const Mesh& mesh, RayHitInfo& hit)
		{
			bool isHit = false;
			float minDistance = FLT_MAX;

			for (size_t i = 0; i < mesh.mIndices.size(); i += 3)
			{
				const Vector3& v0 = mesh.mVertices[mesh.mIndices[i]].Position;
				const Vector3& v1 = mesh.mVertices[mesh.mIndices[i + 1]].Position;
				const Vector3& v2 = mesh.mVertices[mesh.mIndices[i + 2]].Position;

				float t;
				if (Detect(ray, v0, v1, v2, t))
				{
					isHit = true;

					if (t < minDistance)
					{
						minDistance = t;
						hit.distance = t;
					}
				}
			}

			return isHit;
		}

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