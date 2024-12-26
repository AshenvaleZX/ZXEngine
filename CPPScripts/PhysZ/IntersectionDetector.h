#pragma once
#include "../pubh.h"
#include "PhysZEnumStruct.h"

namespace ZXEngine
{
	class Mesh;
	namespace PhysZ
	{
		class Ray;
		class CollisionBox;
		class CollisionPlane;
		class CollisionSphere;
		class CollisionCircle2D;
		class IntersectionDetector
		{
		public:
			static bool Detect(const Ray& ray, const CollisionBox& box);
			static bool Detect(const Ray& ray, const CollisionPlane& plane);
			static bool Detect(const Ray& ray, const CollisionSphere& sphere);
			static bool Detect(const Ray& ray, const CollisionCircle2D& circle);
			static bool Detect(const Ray& ray, const CollisionCircle2D& circle, RayHitInfo& hit);
			static bool Detect(const Ray& ray, const Vector3& v0, const Vector3& v1, const Vector3& v2);
			static bool Detect(const Ray& ray, const Mesh& mesh);
			static bool Detect(const CollisionBox& box1, const CollisionBox& box2);
			static bool Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2);
			static bool Detect(const CollisionBox& box, const CollisionPlane& plane);
			static bool Detect(const CollisionSphere& sphere, const CollisionPlane& plane);

			// 判断两个Box投影到某个轴上会不会有重叠
			static bool IsOverlapOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine);
			// 获取两个Box投影到某个轴上的重叠长度(返回值大于0表示重叠，小于0表示未重叠)
			static float GetPenetrationOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine);
			// 检测两条线段是否相交(交点通过第7个参数获取)
			static bool DetectLineSegmentContact(const Vector3& midPoint1, const Vector3& dir1, float halfLength1, const Vector3& midPoint2, const Vector3& dir2, float halfLength2, Vector3& contactPoint, bool useOne = true);

		private:
			static bool DetectBoxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane);
			static bool DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane);
		};
	}
}