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
		class CollisionDetector
		{
		public:
			static uint32_t Detect(const CollisionBox& box, const CollisionSphere& sphere, CollisionData* data);
			static uint32_t Detect(const CollisionSphere& sphere1, const CollisionSphere& sphere2, CollisionData* data);

			static uint32_t Detect(const CollisionBox& box, const CollisionPlane& plane, CollisionData* data, bool isHalfSpace = true);
			static uint32_t Detect(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data, bool isHalfSpace = true);

		private:
			// ����ƽ����ײ���(��һ���������������άƽ����ײ�����������涼������ײ)
			static uint32_t DetectSphereAndPlane(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);
			// ����ƽ����ײ���(��ƽ�浱��һ���ռ仮�֣�ֻ�ܴ�������ײ�������ȫ�ڱ�����ΪǶ��ƽ���ڣ���Ȼ������ײ)
			static uint32_t DetectSphereAndHalfSpace(const CollisionSphere& sphere, const CollisionPlane& plane, CollisionData* data);
		};
	}
}