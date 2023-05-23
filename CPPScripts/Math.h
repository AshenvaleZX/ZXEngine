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
		static float PIx2;
		// 角度弧度转换
		static float Deg2Rad(float degree);
		static float Rad2Deg(float radian);
		// 随机数
		static int RandomInt(int min = 0, int max = RAND_MAX);
		static float RandomFloat(float min = 0.0f, float max = 1.0f);
		// a和b是否基本相等
		static bool Approximately(float a, float b, float eps = FLT_EPSILON);
		// 计算透视投影矩阵
		static Matrix4 Perspective(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 PerspectiveLH(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 PerspectiveRH(float fov, float aspect, float nearClip, float farClip);
		// 计算正交投影矩阵
		static Matrix4 Orthographic(float left, float right, float bottom, float top);
		static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
		static Matrix4 OrthographicLH(float left, float right, float bottom, float top, float zNear, float zFar);
		static Matrix4 OrthographicRH(float left, float right, float bottom, float top, float zNear, float zFar);
		// 位移
		static Matrix4 Translate(const Matrix4& oriMat, const Vector3& v);
		// 旋转
		static Matrix4 Rotate(const Matrix4& oriMat, float angle, const Vector3& axis);
		// 缩放
		static Matrix4 Scale(const Matrix4& oriMat, const Vector3& scale);
		// 计算逆矩阵
		static Matrix3 Inverse(const Matrix3& mat);
		static Matrix4 Inverse(const Matrix4& mat);
		// 计算转置矩阵
		static Matrix3 Transpose(const Matrix3& mat);
		static Matrix4 Transpose(const Matrix4& mat);
		// 向量点乘
		static float Dot(const Vector3& left, const Vector3& right);
		// 向量叉乘
		static Vector3 Cross(const Vector3& left, const Vector3& right);
		// 获取随机垂直向量
		static Vector2 GetRandomPerpendicular(const Vector2& v);
		static Vector3 GetRandomPerpendicular(const Vector3& v);
		// 这个函数和GLM的LookAt函数效果是一样的
		static Matrix4 GetLookToMatrix(const Vector3& pos, const Vector3& forward, const Vector3& up);
		static float Distance(const Vector2& a, const Vector2& b);
		static float Distance(const Vector3& a, const Vector3& b);

		template<class T>
		static constexpr T Min(T num1, T num2);
		template<class T>
		static constexpr T Max(T num1, T num2);
		template<class T>
		static constexpr T Clamp(T num, T min, T max);
		template<class T>
		static constexpr T AlignUp(T num, T alignment);
		template<class T>
		static constexpr T AlignDown(T num, T alignment);
		template<class T>
		static constexpr T AlignUpPOT(T num, T alignment);
		template<class T>
		static constexpr T AlignDownPOT(T num, T alignment);
	};

	template<class T>
	constexpr T Math::Min(T num1, T num2)
	{
		return num1 < num2 ? num1 : num2;
	}

	template<class T>
	constexpr T Math::Max(T num1, T num2)
	{
		return num1 > num2 ? num1 : num2;
	}

	template<class T>
	constexpr T Math::Clamp(T num, T min, T max)
	{
		return Math::Min(Math::Max(num, min), max);
	}

	template<class T>
	constexpr T Math::AlignUp(T num, T alignment)
	{
		return (num + alignment - 1) / alignment * alignment;
	}

	template<class T>
	constexpr T Math::AlignDown(T num, T alignment)
	{
		return num / alignment * alignment;
	}

	template<class T>
	constexpr T Math::AlignUpPOT(T num, T alignment)
	{
		return (num + alignment - 1) & ~(alignment - 1);
	}

	template<class T>
	constexpr T Math::AlignDownPOT(T num, T alignment)
	{
		return num & ~(alignment - 1);
	}
}