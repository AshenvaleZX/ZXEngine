#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Quaternion
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion() {};

		static Quaternion Euler(float x, float y, float z);

		// 运算符重载
		Quaternion operator *(const Quaternion& q) const;
		void operator *= (const Quaternion& q);

		// 函数声明的后面加const的意思，是表示这个函数不会修改类中的任何数据成员，不加也行。
		// 但是加了的话函数代码里去改就会报错，强行让你不能改，防止写代码的时候不小心去改了，加强健壮性。
		float Normal() const;
		float NormalSquare() const;
		void Normalize();
		vec3 GetEulerAngles() const;
		void SetEulerAngles(float x, float y, float z);
	};
}
