#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		class Ray
		{
		public:
			Vector3 mOrigin;
			Vector3 mDirection;

			Ray() = default;
			Ray(const Vector3& origin, const Vector3& direction);

			Vector3 GetPoint(float distance) const;
		};
	}
}