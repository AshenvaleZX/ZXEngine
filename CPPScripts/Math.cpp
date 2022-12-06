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

	Matrix4 Math::Perspective(float fov, float aspect, float nearClip, float farClip)
	{
		// 默认用左手坐标系的
		return PerspectiveLH(fov, aspect, nearClip, farClip);
	}

	// 基于左手坐标系
	Matrix4 Math::PerspectiveLH(float fov, float aspect, float nearClip, float farClip)
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

		return Matrix4(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33);
	}

	// 基于右手坐标系
	Matrix4 Math::PerspectiveRH(float fov, float aspect, float nearClip, float farClip)
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

		return Matrix4(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33);
	}

	Matrix4 Math::Orthographic(float left, float right, float bottom, float top)
	{
		Matrix4 resMat(1);
		resMat.m00 = 2 / (right - left);
		resMat.m11 = 2 / (top - bottom);
		resMat.m22 = -1;
		resMat.m03 = -(right + left) / (right - left);
		resMat.m13 = -(top + bottom) / (top - bottom);
		return resMat;
	}

	Matrix4 Math::Translate(Matrix4 const& oriMat, Vector3 const& v)
	{
		Matrix4 resMat(oriMat);
		resMat.m03 = oriMat.m03 + v.x;
		resMat.m13 = oriMat.m13 + v.y;
		resMat.m23 = oriMat.m23 + v.z;
		return resMat;
	}

	Matrix4 Math::Rotate(Matrix4 const& oriMat, float angle, Vector3 const& axis)
	{
		float a = angle;
		float c = cos(a);
		float s = sin(a);

		Vector3 temp((1 - c) * axis);

		Matrix4 rotate(1);
		rotate.m00 = c + temp[0] * axis[0];
		rotate.m10 = temp[0] * axis[1] + s * axis[2];
		rotate.m20 = temp[0] * axis[2] - s * axis[1];

		rotate.m01 = temp[1] * axis[0] - s * axis[2];
		rotate.m11 = c + temp[1] * axis[1];
		rotate.m21 = temp[1] * axis[2] + s * axis[0];

		rotate.m02 = temp[2] * axis[0] + s * axis[1];
		rotate.m12 = temp[2] * axis[1] - s * axis[0];
		rotate.m22 = c + temp[2] * axis[2];

		return rotate * oriMat;
	}

	Matrix4 Math::Scale(Matrix4 const& oriMat, Vector3 const& scale)
	{
		Matrix4 resMat(oriMat);
		resMat.m00 *= scale.x;
		resMat.m10 *= scale.x;
		resMat.m20 *= scale.x;

		resMat.m01 *= scale.y;
		resMat.m11 *= scale.y;
		resMat.m21 *= scale.y;

		resMat.m02 *= scale.z;
		resMat.m12 *= scale.z;
		resMat.m22 *= scale.z;
		return resMat;
	}

	Matrix3 Math::Inverse(const Matrix3& mat)
	{
		// 伴随矩阵法
		float oneOverDeterminant = 1.0f / mat.GetDeterminant();

		Matrix3 resMat = Matrix3();
		resMat.m00 = +(mat.m11 * mat.m22 - mat.m12 * mat.m21) * oneOverDeterminant;
		resMat.m01 = -(mat.m10 * mat.m22 - mat.m12 * mat.m20) * oneOverDeterminant;
		resMat.m02 = +(mat.m10 * mat.m21 - mat.m11 * mat.m20) * oneOverDeterminant;

		resMat.m10 = -(mat.m01 * mat.m22 - mat.m02 * mat.m21) * oneOverDeterminant;
		resMat.m11 = +(mat.m00 * mat.m22 - mat.m02 * mat.m20) * oneOverDeterminant;
		resMat.m12 = -(mat.m00 * mat.m21 - mat.m01 * mat.m20) * oneOverDeterminant;

		resMat.m20 = +(mat.m01 * mat.m12 - mat.m02 * mat.m11) * oneOverDeterminant;
		resMat.m21 = -(mat.m00 * mat.m12 - mat.m02 * mat.m10) * oneOverDeterminant;
		resMat.m22 = +(mat.m00 * mat.m11 - mat.m01 * mat.m10) * oneOverDeterminant;

		return resMat;
	}

	Matrix4 Math::Inverse(const Matrix4& mat)
	{
		// 伴随矩阵法
		float inv00, inv01, inv02, inv03, inv10, inv11, inv12, inv13, inv20, inv21, inv22, inv23, inv30, inv31, inv32, inv33;
		inv00 =  mat.m11 * mat.m22 * mat.m33 - mat.m11 * mat.m23 * mat.m32 - mat.m21 * mat.m12 * mat.m33 + mat.m21 * mat.m13 * mat.m32 + mat.m31 * mat.m12 * mat.m23 - mat.m31 * mat.m13 * mat.m22;
		inv10 = -mat.m10 * mat.m22 * mat.m33 + mat.m10 * mat.m23 * mat.m32 + mat.m20 * mat.m12 * mat.m33 - mat.m20 * mat.m13 * mat.m32 - mat.m30 * mat.m12 * mat.m23 + mat.m30 * mat.m13 * mat.m22;
		inv20 =  mat.m10 * mat.m21 * mat.m33 - mat.m10 * mat.m23 * mat.m31 - mat.m20 * mat.m11 * mat.m33 + mat.m20 * mat.m13 * mat.m31 + mat.m30 * mat.m11 * mat.m23 - mat.m30 * mat.m13 * mat.m21;
		inv30 = -mat.m10 * mat.m21 * mat.m32 + mat.m10 * mat.m22 * mat.m31 + mat.m20 * mat.m11 * mat.m32 - mat.m20 * mat.m12 * mat.m31 - mat.m30 * mat.m11 * mat.m22 + mat.m30 * mat.m12 * mat.m21;
		inv01 = -mat.m01 * mat.m22 * mat.m33 + mat.m01 * mat.m23 * mat.m32 + mat.m21 * mat.m02 * mat.m33 - mat.m21 * mat.m03 * mat.m32 - mat.m31 * mat.m02 * mat.m23 + mat.m31 * mat.m03 * mat.m22;
		inv11 =  mat.m00 * mat.m22 * mat.m33 - mat.m00 * mat.m23 * mat.m32 - mat.m20 * mat.m02 * mat.m33 + mat.m20 * mat.m03 * mat.m32 + mat.m30 * mat.m02 * mat.m23 - mat.m30 * mat.m03 * mat.m22;
		inv21 = -mat.m00 * mat.m21 * mat.m33 + mat.m00 * mat.m23 * mat.m31 + mat.m20 * mat.m01 * mat.m33 - mat.m20 * mat.m03 * mat.m31 - mat.m30 * mat.m01 * mat.m23 + mat.m30 * mat.m03 * mat.m21;
		inv31 =  mat.m00 * mat.m21 * mat.m32 - mat.m00 * mat.m22 * mat.m31 - mat.m20 * mat.m01 * mat.m32 + mat.m20 * mat.m02 * mat.m31 + mat.m30 * mat.m01 * mat.m22 - mat.m30 * mat.m02 * mat.m21;
		inv02 =  mat.m01 * mat.m12 * mat.m33 - mat.m01 * mat.m13 * mat.m32 - mat.m11 * mat.m02 * mat.m33 + mat.m11 * mat.m03 * mat.m32 + mat.m31 * mat.m02 * mat.m13 - mat.m31 * mat.m03 * mat.m12;
		inv12 = -mat.m00 * mat.m12 * mat.m33 + mat.m00 * mat.m13 * mat.m32 + mat.m10 * mat.m02 * mat.m33 - mat.m10 * mat.m03 * mat.m32 - mat.m30 * mat.m02 * mat.m13 + mat.m30 * mat.m03 * mat.m12;
		inv22 =  mat.m00 * mat.m11 * mat.m33 - mat.m00 * mat.m13 * mat.m31 - mat.m10 * mat.m01 * mat.m33 + mat.m10 * mat.m03 * mat.m31 + mat.m30 * mat.m01 * mat.m13 - mat.m30 * mat.m03 * mat.m11;
		inv32 = -mat.m00 * mat.m11 * mat.m32 + mat.m00 * mat.m12 * mat.m31 + mat.m10 * mat.m01 * mat.m32 - mat.m10 * mat.m02 * mat.m31 - mat.m30 * mat.m01 * mat.m12 + mat.m30 * mat.m02 * mat.m11;
		inv03 = -mat.m01 * mat.m12 * mat.m23 + mat.m01 * mat.m13 * mat.m22 + mat.m11 * mat.m02 * mat.m23 - mat.m11 * mat.m03 * mat.m22 - mat.m21 * mat.m02 * mat.m13 + mat.m21 * mat.m03 * mat.m12;
		inv13 =  mat.m00 * mat.m12 * mat.m23 - mat.m00 * mat.m13 * mat.m22 - mat.m10 * mat.m02 * mat.m23 + mat.m10 * mat.m03 * mat.m22 + mat.m20 * mat.m02 * mat.m13 - mat.m20 * mat.m03 * mat.m12;
		inv23 = -mat.m00 * mat.m11 * mat.m23 + mat.m00 * mat.m13 * mat.m21 + mat.m10 * mat.m01 * mat.m23 - mat.m10 * mat.m03 * mat.m21 - mat.m20 * mat.m01 * mat.m13 + mat.m20 * mat.m03 * mat.m11;
		inv33 =  mat.m00 * mat.m11 * mat.m22 - mat.m00 * mat.m12 * mat.m21 - mat.m10 * mat.m01 * mat.m22 + mat.m10 * mat.m02 * mat.m21 + mat.m20 * mat.m01 * mat.m12 - mat.m20 * mat.m02 * mat.m11;

		float oneOverDeterminant = 1.0f / (mat.m00 * inv00 + mat.m01 * inv10 + mat.m02 * inv20 + mat.m03 * inv30);

		return Matrix4(
			inv00 * oneOverDeterminant, inv01 * oneOverDeterminant, inv02 * oneOverDeterminant, inv03 * oneOverDeterminant,
			inv10 * oneOverDeterminant, inv11 * oneOverDeterminant, inv12 * oneOverDeterminant, inv13 * oneOverDeterminant,
			inv20 * oneOverDeterminant, inv21 * oneOverDeterminant, inv22 * oneOverDeterminant, inv23 * oneOverDeterminant,
			inv30 * oneOverDeterminant, inv31 * oneOverDeterminant, inv32 * oneOverDeterminant, inv33 * oneOverDeterminant);
	}

	Vector2 Math::Normalize(Vector2 v)
	{
		float l = (float)sqrt(pow(v.x, 2) + pow(v.y, 2));
		return Vector2(v.x/l, v.y/l);
	}

	Vector3 Math::Normalize(Vector3 v)
	{
		float l = (float)sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
		return Vector3(v.x/l, v.y/l, v.z/l);
	}

	Vector4 Math::Normalize(Vector4 v)
	{
		float l = (float)sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2));
		return Vector4(v.x / l, v.y / l, v.z / l, v.w/l);
	}

	float Math::Dot(Vector3 left, Vector3 right)
	{
		return left.x * right.x + left.y + right.y + left.z + right.z;
	}

	Vector3 Math::Cross(Vector3 left, Vector3 right)
	{
		return Vector3(
			left.y * right.z - left.z * right.y,
			left.z * right.x - left.x * right.z,
			left.x * right.y - left.y * right.x);
	}

	Matrix4 Math::GetLookToMatrix(Vector3 pos, Vector3 forward, Vector3 up)
	{
		// 学Unity用的左手坐标系，up叉乘forward得到right，右手坐标系得反过来
		Vector3 right = Cross(up, forward);

		// 基于左手坐标系构建View矩阵这里的forward应该是正的，右手坐标系是负的
		Matrix4 viewMat = Matrix4(
			right.x  , right.y  , right.z  , 0,
			up.x     , up.y     , up.z     , 0,
			forward.x, forward.y, forward.z, 0,
			0        , 0        , 0        , 1);
		Matrix4 posMat = Matrix4(
			1, 0, 0, -pos.x,
			0, 1, 0, -pos.y,
			0, 0, 1, -pos.z,
			0, 0, 0, 1);
		/**
		Matrix4 viewMat = Matrix4(
			right.x, up.x, forward.x, 0,
			right.y, up.y, forward.y, 0,
			right.z, up.z, forward.z, 0,
			0, 0, 0, 1);
		Matrix4 posMat = Matrix4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-pos.x, -pos.y, -pos.z, 1);
		//*/

		return viewMat * posMat;
	}
}