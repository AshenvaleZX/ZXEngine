#include "pubh.h"
#include "Math.h"
#include "Debug.h"

namespace ZXEngine
{
	float Math::PI = 3.141592653f;
	float Math::PIx2 = 6.283185306f;
	float Math::SQRT2 = 1.414213562f;
	float Math::INV_SQRT2 = 0.707106781f;

	Vector3 Math::Slerp(const Vector3& v1, const Vector3& v2, float t)
	{
		float v1Mag = v1.GetMagnitude();
		float v2Mag = v2.GetMagnitude();

		if (v1Mag < 0.00001f || v2Mag < 0.00001f)
			return Lerp(v1, v2, t);

		float lerpMag = Lerp(v1Mag, v2Mag, t);

		float dot = Dot(v1, v2) / (v1Mag * v2Mag);

		// 两个向量几乎一致
		if (dot > 0.99999f)
		{
			return Lerp(v1, v2, t);
		}
		// 两个向量几乎相反
		else if (dot < -0.99999f)
		{
			Vector3 v1Normalized = v1 / v1Mag;
			Vector3 axis = GetPerpendicular(v1Normalized);
			Matrix3 m(axis, PI * t);
			return m * v1Normalized * lerpMag;
		}
		// 一般情况
		else
		{
			float theta = acos(dot) * t;

			Vector3 v1Normalized = v1 / v1Mag;
			Vector3 v2Normalized = v2 / v2Mag;
			Vector3 axis = Cross(v1Normalized, v2Normalized);

			Matrix3 m(axis, theta);
			return m * v1Normalized * lerpMag;
		}
	}

	Quaternion Math::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		Quaternion qt = q2;

		float cosTheta = Dot(q1, q2);

		if (cosTheta < 0.0f)
		{
			qt = -q2;
			cosTheta = -cosTheta;
		}

		float p1, p2;
		// 如果两个四元数非常接近，那么直接线性插值即可
		if (cosTheta > 0.95f)
		{
			p1 = 1.0f - t;
			p2 = t;
		}
		else
		{
			float theta = acos(cosTheta);
			float oneOverSinTheta = 1.0f / sin(theta);

			p1 = sin((1.0f - t) * theta) * oneOverSinTheta;
			p2 = sin(t * theta) * oneOverSinTheta;
		}

		return Quaternion(
			p1 * q1.x + p2 * qt.x,
			p1 * q1.y + p2 * qt.y,
			p1 * q1.z + p2 * qt.z,
			p1 * q1.w + p2 * qt.w);
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

#ifdef ZX_API_VULKAN
		// Vulkan的Y轴要反转一下，否则画面会颠倒
		m11 *= -1;
#endif

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

#ifdef ZX_API_VULKAN
		// Vulkan的Y轴要反转一下，否则画面会颠倒
		resMat.m11 *= -1;
#endif

		return resMat;
	}

	Matrix4 Math::Orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		// 默认用左手坐标系的
		return OrthographicLH(left, right, bottom, top, zNear, zFar);
	}

	Matrix4 Math::OrthographicLH(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		Matrix4 resMat(1);
		resMat.m00 = 2.0f / (right - left);
		resMat.m11 = 2.0f / (top - bottom);
		resMat.m22 = 2.0f / (zFar - zNear);
		resMat.m03 = -(right + left) / (right - left);
		resMat.m13 = -(top + bottom) / (top - bottom);
		resMat.m23 = -(zFar + zNear) / (zFar - zNear);

#ifdef ZX_API_VULKAN
		// Vulkan的Y轴要反转一下，否则画面会颠倒
		resMat.m11 *= -1;
#endif

		return resMat;
	}

	Matrix4 Math::OrthographicRH(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		Matrix4 resMat(1);
		resMat.m00 = 2.0f / (right - left);
		resMat.m11 = 2.0f / (top - bottom);
		resMat.m22 = -2.0f / (zFar - zNear);
		resMat.m03 = -(right + left) / (right - left);
		resMat.m13 = -(top + bottom) / (top - bottom);
		resMat.m23 = -(zFar + zNear) / (zFar - zNear);
		return resMat;
	}

	Matrix4 Math::Translate(const Matrix4& oriMat, const Vector3& v)
	{
		Matrix4 resMat(oriMat);
		resMat.m03 = oriMat.m03 + v.x;
		resMat.m13 = oriMat.m13 + v.y;
		resMat.m23 = oriMat.m23 + v.z;
		return resMat;
	}

	Matrix4 Math::Rotate(const Matrix4& oriMat, float angle, const Vector3& axis)
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

	Matrix4 Math::Scale(const Matrix4& oriMat, const Vector3& scale)
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

	Matrix3 Math::Transpose(const Matrix3& mat)
	{
		return Matrix3(
			mat.m00, mat.m10, mat.m20,
			mat.m01, mat.m11, mat.m21,
			mat.m02, mat.m12, mat.m22);
	}

	Matrix4 Math::Transpose(const Matrix4& mat)
	{
		return Matrix4(
			mat.m00, mat.m10, mat.m20, mat.m30,
			mat.m01, mat.m11, mat.m21, mat.m31,
			mat.m02, mat.m12, mat.m22, mat.m32,
			mat.m03, mat.m13, mat.m23, mat.m33);
	}

	Vector2 Math::GetPerpendicular(const Vector2& v)
	{
		return Vector2(-v.y, v.x).GetNormalized();
	}

	Vector3 Math::GetPerpendicular(const Vector3& v)
	{
		if (fabsf(v.z) > INV_SQRT2)
		{
			float a = v.y * v.y + v.z * v.z;
			float k = 1.0f / sqrtf(a);
			// YZ平面
			return Vector3(0.0f, -v.z * k, v.y * k);
		}
		else
		{
			float a = v.x * v.x + v.y * v.y;
			float k = 1.0f / sqrtf(a);
			// XY平面
			return Vector3(-v.y * k, v.x * k, 0.0f);
		}
	}

	Vector2 Math::GetRandomPerpendicular(const Vector2& v)
	{
		Vector2 res;
		if (v.x != 0.0f)
		{
			res.y = RandomFloat(-1.0f, 1.0f);
			res.x = -(res.y * v.y) / v.x;
		}
		else if (v.y != 0.0f)
		{
			res.x = RandomFloat(-1.0f, 1.0f);
			res.y = -(res.x * v.x) / v.y;
		}
		else
		{
			Debug::LogError("Invalid vector2 !");
		}
		res.Normalize();
		return res;
	}

	Vector3 Math::GetRandomPerpendicular(const Vector3& v)
	{
		Vector3 res;
		if (v.x != 0.0f)
		{
			res.y = RandomFloat(-1.0f, 1.0f);
			res.z = RandomFloat(-1.0f, 1.0f);
			res.x = -(res.y * v.y + res.z * v.z) / v.x;
		}
		else if (v.y != 0.0f)
		{
			res.x = RandomFloat(-1.0f, 1.0f);
			res.z = RandomFloat(-1.0f, 1.0f);
			res.y = -(res.x * v.x + res.z * v.z) / v.y;
		}
		else if (v.z != 0.0f)
		{
			res.x = RandomFloat(-1.0f, 1.0f);
			res.y = RandomFloat(-1.0f, 1.0f);
			res.z = -(res.x * v.x + res.y * v.y) / v.z;
		}
		else
		{
			Debug::LogError("Invalid vector3 !");
		}
		res.Normalize();
		return res;
	}

	Matrix4 Math::GetLookToMatrix(const Vector3& pos, const Vector3& forward, const Vector3& up)
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
		
		return viewMat * posMat;
	}
}