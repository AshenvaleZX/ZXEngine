#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class CollisionData;
		class CollisionSphere;
		class CollisionDetector
		{
		public:
			static uint32_t Detect(CollisionSphere* sphere1, CollisionSphere* sphere2, CollisionData* data);
		};
	}
}