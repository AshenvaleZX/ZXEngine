#include "Ray.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		Ray::Ray(const Vector3& origin, const Vector3& direction) :
			mOrigin(origin), 
			mDirection(direction) 
		{}

		string Ray::ToString() const
		{
			return "Pos: " + mOrigin.ToString() + " Dir: " + mDirection.ToString();
		}

		Vector3 Ray::GetPoint(float distance) const
		{
			return mOrigin + mDirection * distance;
		}
	}
}