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
		// �ǶȻ���ת��
		static float Deg2Rad(float degree);
		static float Rad2Deg(float radian);
		// �����
		static int RandomInt(int min = 0, int max = RAND_MAX);
		static float RandomFloat(float min = 0.0f, float max = 1.0f);
		// a��b�Ƿ�������
		static bool Approximately(float a, float b, float eps = FLT_EPSILON);
		// ����͸��ͶӰ����
		static Matrix4 Perspective(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 PerspectiveLH(float fov, float aspect, float nearClip, float farClip);
		static Matrix4 PerspectiveRH(float fov, float aspect, float nearClip, float farClip);
		// ��������ͶӰ����
		static Matrix4 Orthographic(float left, float right, float bottom, float top);
		static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar);
		static Matrix4 OrthographicLH(float left, float right, float bottom, float top, float zNear, float zFar);
		static Matrix4 OrthographicRH(float left, float right, float bottom, float top, float zNear, float zFar);
		// λ��
		static Matrix4 Translate(const Matrix4& oriMat, const Vector3& v);
		// ��ת
		static Matrix4 Rotate(const Matrix4& oriMat, float angle, const Vector3& axis);
		// ����
		static Matrix4 Scale(const Matrix4& oriMat, const Vector3& scale);
		// ���������
		static Matrix3 Inverse(const Matrix3& mat);
		static Matrix4 Inverse(const Matrix4& mat);
		// ����ת�þ���
		static Matrix3 Transpose(const Matrix3& mat);
		static Matrix4 Transpose(const Matrix4& mat);
		// �������
		static float Dot(const Vector3& left, const Vector3& right);
		// �������
		static Vector3 Cross(const Vector3& left, const Vector3& right);
		// ��ȡ�����ֱ����
		static Vector2 GetRandomPerpendicular(const Vector2& v);
		static Vector3 GetRandomPerpendicular(const Vector3& v);
		// ���������GLM��LookAt����Ч����һ����
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