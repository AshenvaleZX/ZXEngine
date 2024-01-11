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
		static float SQRT2;
		static float INV_SQRT2;

		
		static inline float Deg2Rad(float degree);
		static inline float Rad2Deg(float radian);

		static inline int RandomInt(int min = 0, int max = RAND_MAX);
		static inline float RandomFloat(float min = 0.0f, float max = 1.0f);

		static inline bool Approximately(float a, float b, float eps = FLT_EPSILON);

		static inline float Distance(const Vector2& a, const Vector2& b);
		static inline float Distance(const Vector3& a, const Vector3& b);

		static inline float Dot(const Vector2& left, const Vector2& right);
		static inline float Dot(const Vector3& left, const Vector3& right);
		static inline float Dot(const Vector4& left, const Vector4& right);
		static inline float Dot(const Quaternion& left, const Quaternion& right);
		static inline Vector3 Cross(const Vector3& left, const Vector3& right);

		static inline float Lerp(float v1, float v2, float t);
		static inline Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);
		static inline Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
		static inline Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t);
		static inline Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);

		static inline Matrix4 ScaleMatrix(const Vector3& scale);
		static inline Matrix4 TranslationMatrix(const Vector3& translation);


		// Spherical interpolation
		static Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
		static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);

		// 计算透视投影矩阵
		static Matrix4 Perspective(float fov, float aspect, float nearClip, float farClip);
		// 左手坐标系，Z范围[-1, 1]
		static Matrix4 PerspectiveLHNO(float fov, float aspect, float nearClip, float farClip);
		// 左手坐标系，Z范围[0, 1]
		static Matrix4 PerspectiveLHZO(float fov, float aspect, float nearClip, float farClip);
		// 右手坐标系，Z范围[-1, 1]
		static Matrix4 PerspectiveRHNO(float fov, float aspect, float nearClip, float farClip);
		// 右手坐标系，Z范围[0, 1]
		static Matrix4 PerspectiveRHZO(float fov, float aspect, float nearClip, float farClip);

		// 计算正交投影矩阵
		static Matrix4 Orthographic(float left, float right, float bottom, float top);
		static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
		// 左手坐标系，Z范围[-1, 1]
		static Matrix4 OrthographicLHNO(float left, float right, float bottom, float top, float zNear, float zFar);
		// 左手坐标系，Z范围[0, 1]
		static Matrix4 OrthographicLHZO(float left, float right, float bottom, float top, float zNear, float zFar);
		// 右手坐标系，Z范围[-1, 1]
		static Matrix4 OrthographicRHNO(float left, float right, float bottom, float top, float zNear, float zFar);
		// 右手坐标系，Z范围[0, 1]
		static Matrix4 OrthographicRHZO(float left, float right, float bottom, float top, float zNear, float zFar);

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
		
		// 获取垂直向量
		static Vector2 GetPerpendicular(const Vector2& v);
		static Vector3 GetPerpendicular(const Vector3& v);
		// 获取随机垂直向量
		static Vector2 GetRandomPerpendicular(const Vector2& v);
		static Vector3 GetRandomPerpendicular(const Vector3& v);
		// 这个函数和GLM的LookAt函数效果是一样的
		static Matrix4 GetLookToMatrix(const Vector3& pos, const Vector3& forward, const Vector3& up);


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

	float Math::Deg2Rad(float degree)
	{
		return Math::PI / 180.f * degree;
	}

	float Math::Rad2Deg(float radian)
	{
		return radian / Math::PI * 180.f;
	}

	int Math::RandomInt(int min, int max)
	{
		return rand() % (max - min + 1) + min;
	}

	float Math::RandomFloat(float min, float max)
	{
		return min + float(rand() / double(RAND_MAX)) * (max - min);
	}

	bool Math::Approximately(float a, float b, float eps)
	{
		return fabs(a - b) <= eps;
	}

	float Math::Distance(const Vector2& a, const Vector2& b)
	{
		return (a - b).GetMagnitude();
	}

	float Math::Distance(const Vector3& a, const Vector3& b)
	{
		return (a - b).GetMagnitude();
	}

	float Math::Dot(const Vector2& left, const Vector2& right)
	{
		return left.x * right.x + left.y * right.y;
	}

	float Math::Dot(const Vector3& left, const Vector3& right)
	{
		return left.x * right.x + left.y * right.y + left.z * right.z;
	}

	float Math::Dot(const Vector4& left, const Vector4& right)
	{
		return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
	}

	float Math::Dot(const Quaternion& left, const Quaternion& right)
	{
		return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
	}

	Vector3 Math::Cross(const Vector3& left, const Vector3& right)
	{
		return Vector3(
			left.y * right.z - left.z * right.y,
			left.z * right.x - left.x * right.z,
			left.x * right.y - left.y * right.x);
	}

	float Math::Lerp(float v1, float v2, float t)
	{
		return v1 + (v2 - v1) * t;
	}

	Vector2 Math::Lerp(const Vector2& v1, const Vector2& v2, float t)
	{
		return Vector2(
			v1.x + (v2.x - v1.x) * t,
			v1.y + (v2.y - v1.y) * t);
	}

	Vector3 Math::Lerp(const Vector3& v1, const Vector3& v2, float t)
	{
		return Vector3(
			v1.x + (v2.x - v1.x) * t,
			v1.y + (v2.y - v1.y) * t,
			v1.z + (v2.z - v1.z) * t);
	}

	Vector4 Math::Lerp(const Vector4& v1, const Vector4& v2, float t)
	{
		return Vector4(
			v1.x + (v2.x - v1.x) * t,
			v1.y + (v2.y - v1.y) * t,
			v1.z + (v2.z - v1.z) * t,
			v1.w + (v2.w - v1.w) * t);
	}

	Quaternion Math::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		// 如果点积小于0，说明两个四元数的相距超过360度
		// 但是四元数只有720度的自由度，所以这里插值的时候要取反
		// 
		// 这里的取反操作以及上面两行注释都是参考的网上流传的Unity 4.3的源码
		// 个人感觉这里的意义应该是插值的时候不要超过180度了，超过180度就应该换个方向插值
		// 否则就会出现绕更远的方向去旋转的情况
		if (Dot(q1, q2) < 0.0f)
		{
			return Quaternion(
				q1.x + (-q2.x - q1.x) * t,
				q1.y + (-q2.y - q1.y) * t,
				q1.z + (-q2.z - q1.z) * t,
				q1.w + (-q2.w - q1.w) * t);
		}
		else
		{
			return Quaternion(
				q1.x + (q2.x - q1.x) * t,
				q1.y + (q2.y - q1.y) * t,
				q1.z + (q2.z - q1.z) * t,
				q1.w + (q2.w - q1.w) * t);
		}
	}

	Matrix4 Math::ScaleMatrix(const Vector3& scale)
	{
		return Matrix4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1);
	}

	Matrix4 Math::TranslationMatrix(const Vector3& translation)
	{
		return Matrix4(
			1, 0, 0, translation.x,
			0, 1, 0, translation.y,
			0, 0, 1, translation.z,
			0, 0, 0, 1);
	}

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