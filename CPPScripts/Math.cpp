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
		// 默认用左手坐标系的
		return PerspectiveLH(fov, aspect, nearClip, farClip);
	}

	// 基于左手坐标系
	glm::mat4 Math::PerspectiveLH(float fov, float aspect, float nearClip, float farClip)
	{
		// 参考：https://www.ogldev.org/www/tutorial12/tutorial12.html

		// 第一行
		float m00 = 1 / (aspect * tan(fov * 0.5f));
		float m01 = 0;
		float m02 = 0;
		float m03 = 0;

		// 第二行
		float m10 = 0;
		float m11 = 1 / tan(fov * 0.5f);
		float m12 = 0;
		float m13 = 0;

		// 第三行
		float m20 = 0;
		float m21 = 0;
		float m22 = -(farClip + nearClip) / (nearClip - farClip);
		float m23 = 2 * farClip * nearClip / (nearClip - farClip);

		// 第四行
		float m30 = 0;
		float m31 = 0;
		float m32 = 1;
		float m33 = 0;

		// GLM的行和列是反的
		return glm::mat4(
			m00, m10, m20, m30,
			m01, m11, m21, m31,
			m02, m12, m22, m32,
			m03, m13, m23, m33);
	}

	// 基于右手坐标系
	glm::mat4 Math::PerspectiveRH(float fov, float aspect, float nearClip, float farClip)
	{
		// 第一行
		float m00 = 1 / (aspect * tan(fov * 0.5f));
		float m01 = 0;
		float m02 = 0;
		float m03 = 0;

		// 第二行
		float m10 = 0;
		float m11 = 1 / tan(fov * 0.5f);
		float m12 = 0;
		float m13 = 0;

		// 第三行
		float m20 = 0;
		float m21 = 0;
		float m22 = (farClip + nearClip) / (nearClip - farClip);
		float m23 = 2 * farClip * nearClip / (nearClip - farClip);

		// 第四行
		float m30 = 0;
		float m31 = 0;
		float m32 = -1;
		float m33 = 0;

		// GLM的行和列是反的
		return glm::mat4(
			m00, m10, m20, m30,
			m01, m11, m21, m31,
			m02, m12, m22, m32,
			m03, m13, m23, m33);
	}

	glm::mat4 Math::Orthographic(float left, float right, float bottom, float top)
	{
		glm::mat4 mat(1);
		mat[0][0] = 2 / (right - left);
		mat[1][1] = 2 / (top - bottom);
		mat[2][2] = -1;
		mat[3][0] = -(right + left) / (right - left);
		mat[3][1] = -(top + bottom) / (top - bottom);
		return mat;
	}

	glm::mat4 Math::Translate(glm::mat4 const& oriMat, glm::vec3 const& v)
	{
		glm::mat4 Result(oriMat);
		Result[3] = oriMat[0] * v[0] + oriMat[1] * v[1] + oriMat[2] * v[2] + oriMat[3];
		return Result;
	}

	glm::mat4 Math::Rotate(glm::mat4 const& oriMat, float angle, glm::vec3 const& axis)
	{
		float a = angle;
		float c = cos(a);
		float s = sin(a);

		glm::vec3 temp((1 - c) * axis);

		glm::mat4 Rotate;
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		glm::mat4 Result;
		Result[0] = oriMat[0] * Rotate[0][0] + oriMat[1] * Rotate[0][1] + oriMat[2] * Rotate[0][2];
		Result[1] = oriMat[0] * Rotate[1][0] + oriMat[1] * Rotate[1][1] + oriMat[2] * Rotate[1][2];
		Result[2] = oriMat[0] * Rotate[2][0] + oriMat[1] * Rotate[2][1] + oriMat[2] * Rotate[2][2];
		Result[3] = oriMat[3];
		return Result;
	}

	glm::mat4 Math::Scale(glm::mat4 const& oriMat, glm::vec3 const& scale)
	{
		glm::mat4 Result;
		Result[0] = oriMat[0] * scale[0];
		Result[1] = oriMat[1] * scale[1];
		Result[2] = oriMat[2] * scale[2];
		Result[3] = oriMat[3];
		return Result;
	}

	glm::vec2 Math::Normalize(glm::vec2 v)
	{
		float l = (float)sqrt(pow(v.x, 2) + pow(v.y, 2));
		return glm::vec2(v.x/l, v.y/l);
	}

	glm::vec3 Math::Normalize(glm::vec3 v)
	{
		float l = (float)sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
		return glm::vec3(v.x/l, v.y/l, v.z/l);
	}

	glm::vec4 Math::Normalize(glm::vec4 v)
	{
		float l = (float)sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2));
		return glm::vec4(v.x / l, v.y / l, v.z / l, v.w/l);
	}

	float Math::Dot(glm::vec3 left, glm::vec3 right)
	{
		return left.x * right.x + left.y + right.y + left.z + right.z;
	}

	glm::vec3 Math::Cross(glm::vec3 left, glm::vec3 right)
	{
		return glm::vec3(
			left.y * right.z - left.z * right.y,
			left.z * right.x - left.x * right.z,
			left.x * right.y - left.y * right.x);
	}

	glm::mat4 Math::GetLookToMatrix(glm::vec3 pos, glm::vec3 forward, glm::vec3 up)
	{
		// 学Unity用的左手坐标系，up叉乘forward得到right，右手坐标系得反过来
		glm::vec3 right = Cross(up, forward);

		// 由于GLM里是以列为主序的，平时书面上的矩阵都是以行为主序的，所以这里看起来像是转置过了一样，其实这个viewMat看起来应该是这样的：
		//  Right.x,  Right.y,  Right.z, 0,
		//  Up.x,     Up.y,     Up.z,    0,
		//  Front.x,  Front.y,  Front.z, 0,
		//  0,        0,        0,       1
		// 后面的posMat同理
		// 基于左手坐标系构建View矩阵这里的forward应该是正的，右手坐标系是负的
		glm::mat4 viewMat = glm::mat4(
			right.x, up.x, forward.x, 0,
			right.y, up.y, forward.y, 0,
			right.z, up.z, forward.z, 0,
			0, 0, 0, 1);
		glm::mat4 posMat = glm::mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-pos.x, -pos.y, -pos.z, 1);

		return viewMat * posMat;
	}
}