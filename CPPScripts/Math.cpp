#include "Math.h"

namespace ZXEngine
{
	float Math::PI = 3.1415926f;

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

	glm::mat4 Math::Perspective(float fov, float aspect, float nearClip, float farClip)
	{
		// Ĭ������������ϵ��
		return PerspectiveLH(fov, aspect, nearClip, farClip);
	}

	// ������������ϵ
	glm::mat4 Math::PerspectiveLH(float fov, float aspect, float nearClip, float farClip)
	{
		// �ο���https://www.ogldev.org/www/tutorial12/tutorial12.html

		// ��һ��
		float m00 = 1 / (aspect * tan(fov * 0.5f));
		float m01 = 0;
		float m02 = 0;
		float m03 = 0;

		// �ڶ���
		float m10 = 0;
		float m11 = 1 / tan(fov * 0.5f);
		float m12 = 0;
		float m13 = 0;

		// ������
		float m20 = 0;
		float m21 = 0;
		float m22 = -(farClip + nearClip) / (nearClip - farClip);
		float m23 = 2 * farClip * nearClip / (nearClip - farClip);

		// ������
		float m30 = 0;
		float m31 = 0;
		float m32 = 1;
		float m33 = 0;

		// GLM���к����Ƿ���
		return glm::mat4(
			m00, m10, m20, m30,
			m01, m11, m21, m31,
			m02, m12, m22, m32,
			m03, m13, m23, m33);
	}

	// ������������ϵ
	glm::mat4 Math::PerspectiveRH(float fov, float aspect, float nearClip, float farClip)
	{
		// ��һ��
		float m00 = 1 / (aspect * tan(fov * 0.5f));
		float m01 = 0;
		float m02 = 0;
		float m03 = 0;

		// �ڶ���
		float m10 = 0;
		float m11 = 1 / tan(fov * 0.5f);
		float m12 = 0;
		float m13 = 0;

		// ������
		float m20 = 0;
		float m21 = 0;
		float m22 = (farClip + nearClip) / (nearClip - farClip);
		float m23 = 2 * farClip * nearClip / (nearClip - farClip);

		// ������
		float m30 = 0;
		float m31 = 0;
		float m32 = -1;
		float m33 = 0;

		// GLM���к����Ƿ���
		return glm::mat4(
			m00, m10, m20, m30,
			m01, m11, m21, m31,
			m02, m12, m22, m32,
			m03, m13, m23, m33);
	}
}