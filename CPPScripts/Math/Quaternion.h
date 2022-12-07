#pragma once

namespace ZXEngine
{
	class Vector3;
	class Matrix4;
	class Quaternion
	{
	public:
		static Quaternion Euler(float x, float y, float z);

	public:
		float x;
		float y;
		float z;
		float w;

		Quaternion();
		Quaternion(float x, float y, float z, float w);
		~Quaternion() {};

		// 函数声明的后面加const的意思，是表示这个函数不会修改类中的任何数据成员，不加也行。
		// 但是加了的话函数代码里去改就会报错，强行让你不能改，防止写代码的时候不小心去改了，加强健壮性。
		float Normal() const;
		float NormalSquare() const;
		void Normalize();
		Vector3 GetEulerAngles() const;
		void SetEulerAngles(float x, float y, float z);
		Matrix4 ToMatrix();

		Quaternion operator* (const Quaternion& q) const;
		Quaternion& operator*= (const Quaternion& q);
	};
}
