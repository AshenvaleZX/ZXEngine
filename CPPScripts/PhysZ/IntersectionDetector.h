#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class CollisionBox;
		class CollisionPlane;
		class CollisionSphere;
		class IntersectionDetector
		{
		public:
			static bool Detect(const CollisionBox& box1, const CollisionBox& box2);
			static bool Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2);
			static bool Detect(const CollisionBox& box, const CollisionPlane& plane);
			static bool Detect(const CollisionSphere& sphere, const CollisionPlane& plane);

		private:
			static bool DetectBoxAndHalfSpace(const CollisionBox& box, const CollisionPlane& plane);
			static bool DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane);
			// 判断两个Box投影到某个轴上会不会有重叠
			static bool IsOverlapOnAxis(const CollisionBox& box1, const CollisionBox& box2, const Vector3& axis, const Vector3& centerLine);
		};
	}
}