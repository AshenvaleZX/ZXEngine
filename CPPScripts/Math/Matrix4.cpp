#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace ZXEngine
{
	Matrix4::Matrix4()
	{
		m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f;
		m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
		m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
		m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
	}

	Matrix4::Matrix4(float n)
	{
		m00 = n   ; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f;
		m10 = 0.0f; m11 = n   ; m12 = 0.0f; m13 = 0.0f;
		m20 = 0.0f; m21 = 0.0f; m22 = n   ; m23 = 0.0f;
		m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = n   ;
	}

	Matrix4::Matrix4(const Matrix3& mat3)
	{
		m00 = mat3.m00; m01 = mat3.m01; m02 = mat3.m02; m03 = 0.0f;
		m10 = mat3.m10; m11 = mat3.m11; m12 = mat3.m12; m13 = 0.0f;
		m20 = mat3.m20; m21 = mat3.m21; m22 = mat3.m22; m23 = 0.0f;
		m30 = 0.0f    ; m31 = 0.0f    ; m32 = 0.0f    ; m33 = 1.0f;
	}

	Matrix4::Matrix4(const Matrix4& mat4)
	{
		m00 = mat4.m00; m01 = mat4.m01; m02 = mat4.m02; m03 = mat4.m03;
		m10 = mat4.m10; m11 = mat4.m11; m12 = mat4.m12; m13 = mat4.m13;
		m20 = mat4.m20; m21 = mat4.m21; m22 = mat4.m22; m23 = mat4.m23;
		m30 = mat4.m30; m31 = mat4.m31; m32 = mat4.m32; m33 = mat4.m33;
	}

	Matrix4::Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
		this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
		this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
		this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
	}


	Vector4 Matrix4::operator* (const Vector4& v) const
	{
		float x = m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w;
		float y = m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w;
		float z = m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w;
		float w = m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w;

		return Vector4(x, y, z, w);
	}

	Matrix4 Matrix4::operator+ (const Matrix4& mat) const
	{
		return Matrix4(
			m00 + mat.m00, m01 + mat.m01, m02 + mat.m02, m03 + mat.m03,
			m10 + mat.m10, m11 + mat.m11, m12 + mat.m12, m13 + mat.m13,
			m20 + mat.m20, m21 + mat.m21, m22 + mat.m22, m23 + mat.m23,
			m30 + mat.m30, m31 + mat.m31, m32 + mat.m32, m33 + mat.m33);
	}

	Matrix4 Matrix4::operator- (const Matrix4& mat) const
	{
		return Matrix4(
			m00 - mat.m00, m01 - mat.m01, m02 - mat.m02, m03 - mat.m03,
			m10 - mat.m10, m11 - mat.m11, m12 - mat.m12, m13 - mat.m13,
			m20 - mat.m20, m21 - mat.m21, m22 - mat.m22, m23 - mat.m23,
			m30 - mat.m30, m31 - mat.m31, m32 - mat.m32, m33 - mat.m33);
	}

	Matrix4 Matrix4::operator* (const Matrix4& mat) const
	{
		float m00 = this->m00 * mat.m00 + this->m01 * mat.m10 + this->m02 * mat.m20 + this->m03 * mat.m30;
		float m01 = this->m00 * mat.m01 + this->m01 * mat.m11 + this->m02 * mat.m21 + this->m03 * mat.m31;
		float m02 = this->m00 * mat.m02 + this->m01 * mat.m12 + this->m02 * mat.m22 + this->m03 * mat.m32;
		float m03 = this->m00 * mat.m03 + this->m01 * mat.m13 + this->m02 * mat.m23 + this->m03 * mat.m33;

		float m10 = this->m10 * mat.m00 + this->m11 * mat.m10 + this->m12 * mat.m20 + this->m13 * mat.m30;
		float m11 = this->m10 * mat.m01 + this->m11 * mat.m11 + this->m12 * mat.m21 + this->m13 * mat.m31;
		float m12 = this->m10 * mat.m02 + this->m11 * mat.m12 + this->m12 * mat.m22 + this->m13 * mat.m32;
		float m13 = this->m10 * mat.m03 + this->m11 * mat.m13 + this->m12 * mat.m23 + this->m13 * mat.m33;

		float m20 = this->m20 * mat.m00 + this->m21 * mat.m10 + this->m22 * mat.m20 + this->m23 * mat.m30;
		float m21 = this->m20 * mat.m01 + this->m21 * mat.m11 + this->m22 * mat.m21 + this->m23 * mat.m31;
		float m22 = this->m20 * mat.m02 + this->m21 * mat.m12 + this->m22 * mat.m22 + this->m23 * mat.m32;
		float m23 = this->m20 * mat.m03 + this->m21 * mat.m13 + this->m22 * mat.m23 + this->m23 * mat.m33;

		float m30 = this->m30 * mat.m00 + this->m31 * mat.m10 + this->m32 * mat.m20 + this->m33 * mat.m30;
		float m31 = this->m30 * mat.m01 + this->m31 * mat.m11 + this->m32 * mat.m21 + this->m33 * mat.m31;
		float m32 = this->m30 * mat.m02 + this->m31 * mat.m12 + this->m32 * mat.m22 + this->m33 * mat.m32;
		float m33 = this->m30 * mat.m03 + this->m31 * mat.m13 + this->m32 * mat.m23 + this->m33 * mat.m33;

		return Matrix4(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33);
	}
}