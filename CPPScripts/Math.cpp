#include "Math.h"

namespace ZXEngine
{
	float Math::PI = 3.1415926;

	float Math::Deg2Rad(float degree)
	{
		return Math::PI / 180.f * degree;
	}

	float Math::Rad2Deg(float radian)
	{
		return radian / Math::PI * 180.f;
	}

	bool Math::Approximately(float a, float b, float eps) 
	{
		return fabs(a - b) <= eps;
	}
}