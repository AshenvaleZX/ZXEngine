#include <cassert>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "../Debug.h"

namespace ZXEngine
{
	Vector4::Vector4()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	Vector4::Vector4(float n)
	{
		this->x = n;
		this->y = n;
		this->z = n;
		this->w = n;
	}

	Vector4::Vector4(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	Vector4::Vector4(const Vector2& v2)
	{
		this->x = v2.x;
		this->y = v2.y;
		this->z = 0.0f;
		this->w = 0.0f;
	}

	Vector4::Vector4(const Vector2& v2l, const Vector2& v2r)
	{
		this->x = v2l.x;
		this->y = v2l.y;
		this->z = v2r.x;
		this->w = v2r.y;
	}

	Vector4::Vector4(const Vector2& v2, float z, float w)
	{
		this->x = v2.x;
		this->y = v2.y;
		this->z = z;
		this->w = w;
	}

	Vector4::Vector4(const Vector3& v3)
	{
		this->x = v3.x;
		this->y = v3.y;
		this->z = v3.z;
		this->w = 0.0f;
	}

	Vector4::Vector4(const Vector3& v3, float w)
	{
		this->x = v3.x;
		this->y = v3.y;
		this->z = v3.z;
		this->w = w;
	}

	Vector4::Vector4(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
	}

	std::string Vector4::ToString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
	}

	float& Vector4::operator[] (int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			Debug::LogError("Invalid index to Vector4 !");
			assert(false);
			return x;
			break;
		}
	}

	const float& Vector4::operator[] (int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			Debug::LogError("Invalid index to Vector4 !");
			assert(false);
			return x;
			break;
		}
	}

	Vector4& Vector4::operator= (const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	Vector4 Vector4::operator+ (const Vector4& v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4 Vector4::operator- (const Vector4& v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4 Vector4::operator* (const Vector4& v) const
	{
		return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	Vector4 Vector4::operator/ (const Vector4& v) const
	{
		return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	Vector4 Vector4::operator+= (const Vector4& v) const
	{
		return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vector4 Vector4::operator-= (const Vector4& v) const
	{
		return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	Vector4 Vector4::operator*= (const Vector4& v) const
	{
		return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	Vector4 Vector4::operator/= (const Vector4& v) const
	{
		return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	Vector4 Vector4::operator+ (float n) const
	{
		return Vector4(x + n, y + n, z + n, w + n);
	}

	Vector4 Vector4::operator- (float n) const
	{
		return Vector4(x - n, y - n, z - n, w - n);
	}

	Vector4 Vector4::operator* (float n) const
	{
		return Vector4(x * n, y * n, z * n, w * n);
	}

	Vector4 Vector4::operator/ (float n) const
	{
		return Vector4(x / n, y / n, z / n, w / n);
	}

	Vector4 Vector4::operator+= (float n) const
	{
		return Vector4(x + n, y + n, z + n, w + n);
	}

	Vector4 Vector4::operator-= (float n) const
	{
		return Vector4(x - n, y - n, z - n, w - n);
	}

	Vector4 Vector4::operator*= (float n) const
	{
		return Vector4(x * n, y * n, z * n, w * n);
	}

	Vector4 Vector4::operator/= (float n) const
	{
		return Vector4(x / n, y / n, z / n, w / n);
	}

	Vector4 operator+ (float n, const Vector4& v)
	{
		return Vector4(n + v.x, n + v.y, n + v.z, n + v.w);
	}

	Vector4 operator- (float n, const Vector4& v)
	{
		return Vector4(n - v.x, n - v.y, n - v.z, n - v.w);
	}

	Vector4 operator* (float n, const Vector4& v)
	{
		return Vector4(n * v.x, n * v.y, n * v.z, n * v.w);
	}

	Vector4 operator/ (float n, const Vector4& v)
	{
		return Vector4(n / v.x, n / v.y, n / v.z, n / v.w);
	}

	Vector4 Vector4::operator* (const Matrix4& mat) const
	{
		float x = this->x * mat.m00 + this->y * mat.m10 + this->z * mat.m20 + this->w * mat.m30;
		float y = this->x * mat.m01 + this->y * mat.m11 + this->z * mat.m21 + this->w * mat.m31;
		float z = this->x * mat.m02 + this->y * mat.m12 + this->z * mat.m22 + this->w * mat.m32;
		float w = this->x * mat.m03 + this->y * mat.m13 + this->z * mat.m23 + this->w * mat.m33;

		return Vector4(x, y, z, w);
	}
}