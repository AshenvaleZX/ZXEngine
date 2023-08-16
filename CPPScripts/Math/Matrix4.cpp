#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "../Math.h"
#include "../Debug.h"

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
		m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
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

	float Matrix4::GetDeterminant() const
	{
		// 代数余子式法
		float a00 =  (m00 * m11 * m22) + (m01 * m12 * m20) + (m02 * m10 * m21) - (m02 * m11 * m20) - (m00 * m12 * m21) - (m01 * m10 * m22);
		float a01 = -(m10 * m22 * m33) + (m12 * m23 * m30) + (m13 * m20 * m32) - (m13 * m22 * m30) - (m10 * m23 * m32) - (m12 * m20 * m33);
		float a02 =  (m10 * m21 * m33) + (m11 * m23 * m30) + (m13 * m20 * m31) - (m13 * m21 * m30) - (m10 * m23 * m31) - (m11 * m20 * m33);
		float a03 = -(m10 * m21 * m32) + (m11 * m22 * m30) + (m12 * m20 * m31) - (m12 * m21 * m30) - (m10 * m22 * m31) - (m11 * m20 * m32);
		return m00 * a00 + m01 * a01 + m02 * a02 + m03 * a03;
	}

	Vector4 Matrix4::GetRow(uint32_t index) const
	{
		switch (index)
		{
		case 0:
			return Vector4(m00, m01, m02, m03);
		case 1:
			return Vector4(m10, m11, m12, m13);
		case 2:
			return Vector4(m20, m21, m22, m23);
		case 3:
			return Vector4(m30, m31, m32, m33);
		default:
			Debug::LogError("Get row index out of range.");
			return Vector4();
		}
	}

	Vector4 Matrix4::GetColumn(uint32_t index) const
	{
		switch (index)
		{
		case 0:
			return Vector4(m00, m10, m20, m30);
		case 1:
			return Vector4(m01, m11, m21, m31);
		case 2:
			return Vector4(m02, m12, m22, m32);
		case 3:
			return Vector4(m03, m13, m23, m33);
		default:
			Debug::LogError("Get column index out of range.");
			return Vector4();
		}
	}

	void Matrix4::ToRowMajorArray(float* array) const
	{
		array[0]  = m00; array[1]  = m01; array[2]  = m02; array[3]  = m03;
		array[4]  = m10; array[5]  = m11; array[6]  = m12; array[7]  = m13;
		array[8]  = m20; array[9]  = m21; array[10] = m22; array[11] = m23;
		array[12] = m30; array[13] = m31; array[14] = m32; array[15] = m33;
	}

	void Matrix4::ToColumnMajorArray(float* array) const
	{
		array[0]  = m00; array[1]  = m10; array[2]  = m20; array[3]  = m30;
		array[4]  = m01; array[5]  = m11; array[6]  = m21; array[7]  = m31;
		array[8]  = m02; array[9]  = m12; array[10] = m22; array[11] = m32;
		array[12] = m03; array[13] = m13; array[14] = m23; array[15] = m33;
	}

	std::string Matrix4::ToString() const
	{
		return "\n"
			+ std::to_string(m00) + ", " + std::to_string(m01) + ", " + std::to_string(m02) + ", " + std::to_string(m03) + "\n"
			+ std::to_string(m10) + ", " + std::to_string(m11) + ", " + std::to_string(m12) + ", " + std::to_string(m13) + "\n"
			+ std::to_string(m20) + ", " + std::to_string(m21) + ", " + std::to_string(m22) + ", " + std::to_string(m23) + "\n"
			+ std::to_string(m30) + ", " + std::to_string(m31) + ", " + std::to_string(m32) + ", " + std::to_string(m33) + "\n";
	}

	Matrix4& Matrix4::operator= (const Matrix4& mat)
	{
		m00 = mat.m00; m01 = mat.m01; m02 = mat.m02; m03 = mat.m03;
		m10 = mat.m10; m11 = mat.m11; m12 = mat.m12; m13 = mat.m13;
		m20 = mat.m20; m21 = mat.m21; m22 = mat.m22; m23 = mat.m23;
		m30 = mat.m30; m31 = mat.m31; m32 = mat.m32; m33 = mat.m33;

		return *this;
	}

	bool Matrix4::operator== (const Matrix4& mat) const
	{
		return Math::Approximately(m00, mat.m00) && Math::Approximately(m01, mat.m01) && Math::Approximately(m02, mat.m02) && Math::Approximately(m03, mat.m03)
			&& Math::Approximately(m10, mat.m10) && Math::Approximately(m11, mat.m11) && Math::Approximately(m12, mat.m12) && Math::Approximately(m13, mat.m13)
			&& Math::Approximately(m20, mat.m20) && Math::Approximately(m21, mat.m21) && Math::Approximately(m22, mat.m22) && Math::Approximately(m23, mat.m23)
			&& Math::Approximately(m30, mat.m30) && Math::Approximately(m31, mat.m31) && Math::Approximately(m32, mat.m32) && Math::Approximately(m33, mat.m33);
	}

	bool Matrix4::operator!= (const Matrix4& mat) const
	{
		return !(*this == mat);
	}

	Matrix4 Matrix4::operator- () const
	{
		return Matrix4(
			-m00, -m01, -m02, -m03,
			-m10, -m11, -m12, -m13,
			-m20, -m21, -m22, -m23,
			-m30, -m31, -m32, -m33);
	}

	Matrix4 Matrix4::operator* (float n) const
	{
		return Matrix4(
			m00 * n, m01 * n, m02 * n, m03 * n,
			m10 * n, m11 * n, m12 * n, m13 * n,
			m20 * n, m21 * n, m22 * n, m23 * n,
			m30 * n, m31 * n, m32 * n, m33 * n);
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

	Matrix4& Matrix4::operator*= (float n)
	{
		m00 *= n; m01 *= n; m02 *= n; m03 *= n;
		m10 *= n; m11 *= n; m12 *= n; m13 *= n;
		m20 *= n; m21 *= n; m22 *= n; m23 *= n;
		m30 *= n; m31 *= n; m32 *= n; m33 *= n;

		return *this;
	}

	Matrix4& Matrix4::operator+= (const Matrix4& mat)
	{
		m00 += mat.m00; m01 += mat.m01; m02 += mat.m02; m03 += mat.m03;
		m10 += mat.m10; m11 += mat.m11; m12 += mat.m12; m13 += mat.m13;
		m20 += mat.m20; m21 += mat.m21; m22 += mat.m22; m23 += mat.m23;
		m30 += mat.m30; m31 += mat.m31; m32 += mat.m32; m33 += mat.m33;

		return *this;
	}

	Matrix4& Matrix4::operator-= (const Matrix4& mat)
	{
		m00 -= mat.m00; m01 -= mat.m01; m02 -= mat.m02; m03 -= mat.m03;
		m10 -= mat.m10; m11 -= mat.m11; m12 -= mat.m12; m13 -= mat.m13;
		m20 -= mat.m20; m21 -= mat.m21; m22 -= mat.m22; m23 -= mat.m23;
		m30 -= mat.m30; m31 -= mat.m31; m32 -= mat.m32; m33 -= mat.m33;

		return *this;
	}

	Matrix4& Matrix4::operator*= (const Matrix4& mat)
	{
		float tmp_m00 = m00 * mat.m00 + m01 * mat.m10 + m02 * mat.m20 + m03 * mat.m30;
		float tmp_m01 = m00 * mat.m01 + m01 * mat.m11 + m02 * mat.m21 + m03 * mat.m31;
		float tmp_m02 = m00 * mat.m02 + m01 * mat.m12 + m02 * mat.m22 + m03 * mat.m32;
		float tmp_m03 = m00 * mat.m03 + m01 * mat.m13 + m02 * mat.m23 + m03 * mat.m33;

		float tmp_m10 = m10 * mat.m00 + m11 * mat.m10 + m12 * mat.m20 + m13 * mat.m30;
		float tmp_m11 = m10 * mat.m01 + m11 * mat.m11 + m12 * mat.m21 + m13 * mat.m31;
		float tmp_m12 = m10 * mat.m02 + m11 * mat.m12 + m12 * mat.m22 + m13 * mat.m32;
		float tmp_m13 = m10 * mat.m03 + m11 * mat.m13 + m12 * mat.m23 + m13 * mat.m33;

		float tmp_m20 = m20 * mat.m00 + m21 * mat.m10 + m22 * mat.m20 + m23 * mat.m30;
		float tmp_m21 = m20 * mat.m01 + m21 * mat.m11 + m22 * mat.m21 + m23 * mat.m31;
		float tmp_m22 = m20 * mat.m02 + m21 * mat.m12 + m22 * mat.m22 + m23 * mat.m32;
		float tmp_m23 = m20 * mat.m03 + m21 * mat.m13 + m22 * mat.m23 + m23 * mat.m33;

		float tmp_m30 = m30 * mat.m00 + m31 * mat.m10 + m32 * mat.m20 + m33 * mat.m30;
		float tmp_m31 = m30 * mat.m01 + m31 * mat.m11 + m32 * mat.m21 + m33 * mat.m31;
		float tmp_m32 = m30 * mat.m02 + m31 * mat.m12 + m32 * mat.m22 + m33 * mat.m32;
		float tmp_m33 = m30 * mat.m03 + m31 * mat.m13 + m32 * mat.m23 + m33 * mat.m33;

		m00 = tmp_m00; m01 = tmp_m01; m02 = tmp_m02; m03 = tmp_m03;
		m10 = tmp_m10; m11 = tmp_m11; m12 = tmp_m12; m13 = tmp_m13;
		m20 = tmp_m20; m21 = tmp_m21; m22 = tmp_m22; m23 = tmp_m23;
		m30 = tmp_m30; m31 = tmp_m31; m32 = tmp_m32; m33 = tmp_m33;

		return *this;
	}

	Vector4 Matrix4::operator* (const Vector4& v) const
	{
		float x = m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w;
		float y = m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w;
		float z = m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w;
		float w = m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w;

		return Vector4(x, y, z, w);
	}
}