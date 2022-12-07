#include <cassert>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "../Debug.h"

namespace ZXEngine
{
	Vector3::Vector3()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	Vector3::Vector3(float n)
	{
		this->x = n;
		this->y = n;
		this->z = n;
	}

	Vector3::Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vector3::Vector3(const Vector2& v2)
	{
		this->x = v2.x;
		this->y = v2.y;
		this->z = 0.0f;
	}

	Vector3::Vector3(const Vector2& v2, float z)
	{
		this->x = v2.x;
		this->y = v2.y;
		this->z = z;
	}

	Vector3::Vector3(const Vector3& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	Vector3::Vector3(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	void Vector3::ToArray(float* array)
	{
		array[0] = x;
		array[1] = y;
		array[2] = z;
	}

	std::string Vector3::ToString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}

	float& Vector3::operator[] (int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			Debug::LogError("Invalid index to Vector3 !");
			assert(false);
			return x;
			break;
		}
	}

	const float& Vector3::operator[] (int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			Debug::LogError("Invalid index to Vector3 !");
			assert(false);
			return x;
			break;
		}
	}

	Vector3& Vector3::operator= (const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector3 Vector3::operator+ (const Vector3& v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 Vector3::operator- (const Vector3& v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 Vector3::operator* (const Vector3& v) const
	{
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	Vector3 Vector3::operator/ (const Vector3& v) const
	{
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	Vector3& Vector3::operator+= (const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector3& Vector3::operator-= (const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	Vector3& Vector3::operator*= (const Vector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	Vector3& Vector3::operator/= (const Vector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	Vector3 Vector3::operator+ (float n) const
	{
		return Vector3(x + n, y + n, z + n);
	}

	Vector3 Vector3::operator- (float n) const
	{
		return Vector3(x - n, y - n, z - n);
	}

	Vector3 Vector3::operator* (float n) const
	{
		return Vector3(x * n, y * n, z * n);
	}

	Vector3 Vector3::operator/ (float n) const
	{
		return Vector3(x / n, y / n, z / n);
	}

	Vector3& Vector3::operator+= (float n)
	{
		x += n;
		y += n;
		z += n;
		return *this;
	}

	Vector3& Vector3::operator-= (float n)
	{
		x -= n;
		y -= n;
		z -= n;
		return *this;
	}

	Vector3& Vector3::operator*= (float n)
	{
		x *= n;
		y *= n;
		z *= n;
		return *this;
	}

	Vector3& Vector3::operator/= (float n)
	{
		x /= n;
		y /= n;
		z /= n;
		return *this;
	}

	Vector3 operator+ (float n, const Vector3& v)
	{
		return Vector3(n + v.x, n + v.y, n + v.z);
	}

	Vector3 operator- (float n, const Vector3& v)
	{
		return Vector3(n - v.x, n - v.y, n - v.z);
	}

	Vector3 operator* (float n, const Vector3& v)
	{
		return Vector3(n * v.x, n * v.y, n * v.z);
	}

	Vector3 operator/ (float n, const Vector3& v)
	{
		return Vector3(n / v.x, n / v.y, n / v.z);
	}

	Vector3 Vector3::operator* (const Matrix3& mat) const
	{
		float x = this->x * mat.m00 + this->y * mat.m10 + this->z * mat.m20;
		float y = this->x * mat.m01 + this->y * mat.m11 + this->z * mat.m21;
		float z = this->x * mat.m02 + this->y * mat.m12 + this->z * mat.m22;

		return Vector3(x, y, z);
	}
}