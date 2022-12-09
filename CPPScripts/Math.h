#pragma once
#include <cmath>
#include <cfloat>
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace ZXEngine
{
	class Math
	{
	public:
		static float PI;
		static float Deg2Rad(float degree);
		static float Rad2Deg(float radian);
		// a和b是否基本相等
		static bool Approximately(float a, float b, float eps = FLT_EPSILON);
		static Matrix4 Perspective(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 PerspectiveLH(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 PerspectiveRH(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 Orthographic(float left, float right, float bottom, float top);
		static Matrix4 Translate(const Matrix4& oriMat, const Vector3& v);
		static Matrix4 Rotate(const Matrix4& oriMat, float angle, const Vector3& axis);
		static Matrix4 Scale(const Matrix4& oriMat, const Vector3& scale);
		static Matrix3 Inverse(const Matrix3& mat);
		static Matrix4 Inverse(const Matrix4& mat);
		static float Dot(const Vector3& left, const Vector3& right);
		static Vector3 Cross(const Vector3& left, const Vector3& right);
		// 这个函数和GLM的LookAt函数效果是一样的
		static Matrix4 GetLookToMatrix(const Vector3& pos, const Vector3& forward, const Vector3& up);

		template<class T>
		static T Min(T num1, T num2);
		template<class T>
		static T Max(T num1, T num2);
		template<class T>
		static T Clamp(T num, T min, T max);
	};

	template<class T>
	T Math::Min(T num1, T num2)
	{
		return num1 < num2 ? num1 : num2;
	}

	template<class T>
	T Math::Max(T num1, T num2)
	{
		return num1 > num2 ? num1 : num2;
	}

	template<class T>
	T Math::Clamp(T num, T min, T max)
	{
		if (min > max)
			return num;

		if (num > max)
			num = max;
		if (num < min)
			num = min;

		return num;
	}
}