#pragma once
#include <cmath>
#include <cfloat>
#include "glm/glm.hpp"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"

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
		static glm::mat4 Perspective(float fov, float aspect, float nearClip, float farClip);
		static glm::mat4 PerspectiveLH(float fov, float aspect, float nearClip, float farClip);
		static glm::mat4 PerspectiveRH(float fov, float aspect, float nearClip, float farClip);
		static glm::mat4 Orthographic(float left, float right, float bottom, float top);
		static glm::mat4 Translate(glm::mat4 const& oriMat, glm::vec3 const& v);
		static glm::mat4 Rotate(glm::mat4 const& oriMat, float angle, glm::vec3 const& axis);
		static glm::mat4 Scale(glm::mat4 const& oriMat, glm::vec3 const& scale);
		static glm::vec2 Normalize(glm::vec2 v);
		static glm::vec3 Normalize(glm::vec3 v);
		static glm::vec4 Normalize(glm::vec4 v);
		static float Dot(glm::vec3 left, glm::vec3 right);
		static glm::vec3 Cross(glm::vec3 left, glm::vec3 right);
		// 这个函数和GLM的LookAt函数效果是一样的
		static glm::mat4 GetLookToMatrix(glm::vec3 pos, glm::vec3 forward, glm::vec3 up);

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