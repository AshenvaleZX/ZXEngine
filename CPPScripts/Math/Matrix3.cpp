#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "../Math.h"
#include "../Debug.h"

namespace ZXEngine
{
	Matrix3::Matrix3()
	{
		m00 = 1.0f; m01 = 0.0f; m02 = 0.0f;
		m10 = 0.0f; m11 = 1.0f; m12 = 0.0f;
		m20 = 0.0f; m21 = 0.0f; m22 = 1.0f;
	}

	Matrix3::Matrix3(float n)
	{
		m00 = n   ; m01 = 0.0f; m02 = 0.0f;
		m10 = 0.0f; m11 = n   ; m12 = 0.0f;
		m20 = 0.0f; m21 = 0.0f; m22 = n   ;
	}

	Matrix3::Matrix3(const Matrix3& mat3)
	{
		m00 = mat3.m00; m01 = mat3.m01; m02 = mat3.m02;
		m10 = mat3.m10; m11 = mat3.m11; m12 = mat3.m12;
		m20 = mat3.m20; m21 = mat3.m21; m22 = mat3.m22;
	}

	Matrix3::Matrix3(const Matrix4& mat4)
	{
		m00 = mat4.m00; m01 = mat4.m01; m02 = mat4.m02;
		m10 = mat4.m10; m11 = mat4.m11; m12 = mat4.m12;
		m20 = mat4.m20; m21 = mat4.m21; m22 = mat4.m22;
	}

	Matrix3::Matrix3(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		m00 = v1.x; m01 = v1.y; m02 = v1.z;
		m10 = v2.x; m11 = v2.y; m12 = v2.z;
		m20 = v3.x; m21 = v3.y; m22 = v3.z;
	}

	Matrix3::Matrix3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		this->m00 = m00; this->m01 = m01; this->m02 = m02;
		this->m10 = m10; this->m11 = m11; this->m12 = m12;
		this->m20 = m20; this->m21 = m21; this->m22 = m22;
	}

	Matrix3::Matrix3(const Vector3& v)
	{
		m00 =  0.0f; m01 = -v.z ; m02 =  v.y ;
		m10 =  v.z ; m11 =  0.0f; m12 = -v.x ;
		m20 = -v.y ; m21 =  v.x ; m22 =  0.0f;
	}

	Matrix3::Matrix3(const Vector3& axis, float angle)
	{
		// 此算法先是参考的Unity 4.3，发现Unity 4.3又是参考的Mesa OpenGL
		// 同时GPT给出的算法也是这样的
		float c = cosf(angle);
		float s = sinf(angle);
		float t = 1.0f - c;

		float x = axis.x;
		float y = axis.y;
		float z = axis.z;

		m00 = t * x * x + c;		m01 = t * x * y - s * z;	m02 = t * x * z + s * y;
		m10 = t * x * y + s * z;	m11 = t * y * y + c;		m12 = t * y * z - s * x;
		m20 = t * x * z - s * y;	m21 = t * y * z + s * x;	m22 = t * z * z + c;
	}

	float Matrix3::GetDeterminant() const
	{
		return (m00 * m11 * m22) + (m01 * m12 * m20) + (m02 * m10 * m21) - 
			(m02 * m11 * m20) - (m00 * m12 * m21) - (m01 * m10 * m22);
	}

	float Matrix3::Get(uint32_t row, uint32_t col) const
	{
		return m[row][col];
	}

	Vector3 Matrix3::GetRow(uint32_t index) const
	{
		switch (index)
		{
		case 0:
			return Vector3(m00, m01, m02);
		case 1:
			return Vector3(m10, m11, m12);
		case 2:
			return Vector3(m20, m21, m22);
		default:
			Debug::LogError("Get row index out of range.");
			return Vector3();
		}
	}

	Vector3 Matrix3::GetColumn(uint32_t index) const
	{
		switch (index)
		{
		case 0:
			return Vector3(m00, m10, m20);
		case 1:
			return Vector3(m01, m11, m21);
		case 2:
			return Vector3(m02, m12, m22);
		default:
			Debug::LogError("Get column index out of range.");
			return Vector3();
		}
	}

	void Matrix3::Transpose()
	{
		std::swap(m01, m10);
		std::swap(m02, m20);
		std::swap(m12, m21);
	}

	void Matrix3::ToRowMajorArray(float* array) const
	{
		array[0] = m00; array[1] = m01; array[2] = m02;
		array[3] = m10; array[4] = m11; array[5] = m12;
		array[6] = m20; array[7] = m21; array[8] = m22;
	}

	void Matrix3::ToColumnMajorArray(float* array) const
	{
		array[0] = m00; array[1] = m10; array[2] = m20;
		array[3] = m01; array[4] = m11; array[5] = m21;
		array[6] = m02; array[7] = m12; array[8] = m22;
	}

	std::string Matrix3::ToString() const
	{
		return "\n"
			+ std::to_string(m00) + ", " + std::to_string(m01) + ", " + std::to_string(m02) + "\n"
			+ std::to_string(m10) + ", " + std::to_string(m11) + ", " + std::to_string(m12) + "\n"
			+ std::to_string(m20) + ", " + std::to_string(m21) + ", " + std::to_string(m22) + "\n";
	}

	Matrix3& Matrix3::operator= (const Matrix3& mat)
	{
		m00 = mat.m00; m01 = mat.m01; m02 = mat.m02;
		m10 = mat.m10; m11 = mat.m11; m12 = mat.m12;
		m20 = mat.m20; m21 = mat.m21; m22 = mat.m22;

		return *this;
	}

	bool Matrix3::operator== (const Matrix3& mat) const
	{
		return Math::Approximately(m00, mat.m00) && Math::Approximately(m01, mat.m01) && Math::Approximately(m02, mat.m02)
			&& Math::Approximately(m10, mat.m10) && Math::Approximately(m11, mat.m11) && Math::Approximately(m12, mat.m12)
			&& Math::Approximately(m20, mat.m20) && Math::Approximately(m21, mat.m21) && Math::Approximately(m22, mat.m22);
	}

	bool Matrix3::operator!= (const Matrix3& mat) const
	{
		return !(*this == mat);
	}
	
	Matrix3 Matrix3::operator- () const
	{
		return Matrix3(
			-m00, -m01, -m02,
			-m10, -m11, -m12,
			-m20, -m21, -m22);
	}

	Matrix3 Matrix3::operator* (float n) const
	{
		return Matrix3(
			m00 * n, m01 * n, m02 * n,
			m10 * n, m11 * n, m12 * n,
			m20 * n, m21 * n, m22 * n);
	}

	Matrix3 Matrix3::operator+ (const Matrix3& mat) const
	{
		return Matrix3(
			m00 + mat.m00, m01 + mat.m01, m02 + mat.m02,
			m10 + mat.m10, m11 + mat.m11, m12 + mat.m12,
			m20 + mat.m20, m21 + mat.m21, m22 + mat.m22);
	}

	Matrix3 Matrix3::operator- (const Matrix3& mat) const
	{
		return Matrix3(
			m00 - mat.m00, m01 - mat.m01, m02 - mat.m02,
			m10 - mat.m10, m11 - mat.m11, m12 - mat.m12,
			m20 - mat.m20, m21 - mat.m21, m22 - mat.m22);
	}

	Matrix3 Matrix3::operator* (const Matrix3& mat) const
	{
		float m00 = this->m00 * mat.m00 + this->m01 * mat.m10 + this->m02 * mat.m20;
		float m01 = this->m00 * mat.m01 + this->m01 * mat.m11 + this->m02 * mat.m21;
		float m02 = this->m00 * mat.m02 + this->m01 * mat.m12 + this->m02 * mat.m22;

		float m10 = this->m10 * mat.m00 + this->m11 * mat.m10 + this->m12 * mat.m20;
		float m11 = this->m10 * mat.m01 + this->m11 * mat.m11 + this->m12 * mat.m21;
		float m12 = this->m10 * mat.m02 + this->m11 * mat.m12 + this->m12 * mat.m22;

		float m20 = this->m20 * mat.m00 + this->m21 * mat.m10 + this->m22 * mat.m20;
		float m21 = this->m20 * mat.m01 + this->m21 * mat.m11 + this->m22 * mat.m21;
		float m22 = this->m20 * mat.m02 + this->m21 * mat.m12 + this->m22 * mat.m22;

		return Matrix3(
			m00, m01, m02,
			m10, m11, m12,
			m20, m21, m22);
	}

	Matrix3& Matrix3::operator*= (float n)
	{
		m00 *= n; m01 *= n; m02 *= n;
		m10 *= n; m11 *= n; m12 *= n;
		m20 *= n; m21 *= n; m22 *= n;

		return *this;
	}

	Matrix3& Matrix3::operator+= (const Matrix3& mat)
	{
		m00 += mat.m00; m01 += mat.m01; m02 += mat.m02;
		m10 += mat.m10; m11 += mat.m11; m12 += mat.m12;
		m20 += mat.m20; m21 += mat.m21; m22 += mat.m22;

		return *this;
	}

	Matrix3& Matrix3::operator-= (const Matrix3& mat)
	{
		m00 -= mat.m00; m01 -= mat.m01; m02 -= mat.m02;
		m10 -= mat.m10; m11 -= mat.m11; m12 -= mat.m12;
		m20 -= mat.m20; m21 -= mat.m21; m22 -= mat.m22;

		return *this;
	}

	Matrix3& Matrix3::operator*= (const Matrix3& mat)
	{
		float tmp_m00 = m00 * mat.m00 + m01 * mat.m10 + m02 * mat.m20;
		float tmp_m01 = m00 * mat.m01 + m01 * mat.m11 + m02 * mat.m21;
		float tmp_m02 = m00 * mat.m02 + m01 * mat.m12 + m02 * mat.m22;

		float tmp_m10 = m10 * mat.m00 + m11 * mat.m10 + m12 * mat.m20;
		float tmp_m11 = m10 * mat.m01 + m11 * mat.m11 + m12 * mat.m21;
		float tmp_m12 = m10 * mat.m02 + m11 * mat.m12 + m12 * mat.m22;

		float tmp_m20 = m20 * mat.m00 + m21 * mat.m10 + m22 * mat.m20;
		float tmp_m21 = m20 * mat.m01 + m21 * mat.m11 + m22 * mat.m21;
		float tmp_m22 = m20 * mat.m02 + m21 * mat.m12 + m22 * mat.m22;

		m00 = tmp_m00; m01 = tmp_m01; m02 = tmp_m02;
		m10 = tmp_m10; m11 = tmp_m11; m12 = tmp_m12;
		m20 = tmp_m20; m21 = tmp_m21; m22 = tmp_m22;

		return *this;
	}

	Vector3 Matrix3::operator* (const Vector3& v) const
	{
		float x = m00 * v.x + m01 * v.y + m02 * v.z;
		float y = m10 * v.x + m11 * v.y + m12 * v.z;
		float z = m20 * v.x + m21 * v.y + m22 * v.z;

		return Vector3(x, y, z);
	}

	Matrix3 operator* (float n, const Matrix3& mat)
	{
		return Matrix3(
			mat.m00 * n, mat.m01 * n, mat.m02 * n,
			mat.m10 * n, mat.m11 * n, mat.m12 * n,
			mat.m20 * n, mat.m21 * n, mat.m22 * n);
	}
}