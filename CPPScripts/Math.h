#pragma once
#include <cmath>
#include <cfloat>

namespace ZXEngine
{
	class Math
	{
	public:
		static float PI;
		static float Deg2Rad(float degree);
		static float Rad2Deg(float radian);
		// a��b�Ƿ�������
		static bool Approximately(float a, float b, float eps = FLT_EPSILON);
	};
}