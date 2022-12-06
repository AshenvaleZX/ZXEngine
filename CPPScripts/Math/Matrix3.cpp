#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix4.h"

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

	Matrix3::Matrix3(
		float m00, float m01, float m02,
		float m10, float m11, float m12,
		float m20, float m21, float m22)
	{
		this->m00 = m00; this->m01 = m01; this->m02 = m02;
		this->m10 = m10; this->m11 = m11; this->m12 = m12;
		this->m20 = m20; this->m21 = m21; this->m22 = m22;
	}

	float Matrix3::GetDeterminant() const
	{
		return (m00 * m11 * m22) + (m01 * m12 * m20) + (m02 * m10 * m21) - 
			(m02 * m11 * m20) - (m00 * m12 * m21) - (m01 * m10 * m22);
	}

	std::string Matrix3::ToString()
	{
		return "\n" +
			std::to_string(m00) + ", " + std::to_string(m01) + ", " + std::to_string(m02) + "\n" +
			std::to_string(m10) + ", " + std::to_string(m11) + ", " + std::to_string(m12) + "\n" +
			std::to_string(m20) + ", " + std::to_string(m21) + ", " + std::to_string(m22) + "\n" +
			+"\n";
	}

	Vector3 Matrix3::operator* (const Vector3& v) const
	{
		float x = m00 * v.x + m01 * v.y + m02 * v.z;
		float y = m10 * v.x + m11 * v.y + m12 * v.z;
		float z = m20 * v.x + m21 * v.y + m22 * v.z;

		return Vector3(x, y, z);
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
}