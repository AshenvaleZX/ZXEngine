#include "Ray.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		Ray::Ray(const Vector3& origin, const Vector3& direction) :
			mOrigin(origin), 
			mDirection(direction) 
		{}

		Vector3 Ray::GetPoint(float distance) const
		{
			return mOrigin + mDirection * distance;
		}
	}
}