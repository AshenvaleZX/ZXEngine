#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class CollisionData;
		class CollisionBox;
		class CollisionPlane;
		class CollisionSphere;
		class CollisionPrimitive;
		class CollisionDetector
		{
		public:
			static uint32_t Detect(const CollisionPrimitive* p1, const CollisionPrimitive* p2, CollisionData* data);

			static uint32_t Detect(const CollisionBox& box1, const CollisionBox& box2, CollisionData* data);
			static uint32_t Detect(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data);
			static uint32_t Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2, CollisionData* data);

			static uint32_t Detect(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data, bool isHalfSpace = true);
			static uint32_t Detect(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data, bool isHalfSpace = true);

		private:
			// 球与平面碰撞检测(与一个真正的无体积二维平面碰撞，从正反两面都可以碰撞)
			static uint32_t DetectSphereAndPlane(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);
			// 球与平面碰撞检测(把平面当作一个空间划分，只能从正面碰撞，如果完全在背面视为嵌入平面内，依然产生碰撞)
			static uint32_t DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);
		};
	}
}