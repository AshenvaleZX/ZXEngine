#include <cassert>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "../Math.h"
#include "../Debug.h"

namespace ZXEngine
{
	const Vector4 Vector4::Zero = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 Vector4::One  = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

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

	Vector4::Vector4(float x, const Vector2& v2, float w)
	{
		this->x = x;
		this->y = v2.x;
		this->z = v2.y;
		this->w = w;
	}

	Vector4::Vector4(float x, float y, const Vector2& v2)
	{
		this->x = x;
		this->y = y;
		this->z = v2.x;
		this->w = v2.y;
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

	Vector4::Vector4(float x, const Vector3& v3)
	{
		this->x = x;
		this->y = v3.x;
		this->z = v3.y;
		this->w = v3.z;
	}

	Vector4::Vector4(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;
	}

	void Vector4::Normalize()
	{
		float l = sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2) + powf(w, 2));
		x /= l;
		y /= l;
		z /= l;
		w /= l;
	}

	Vector4 Vector4::GetNormalized() const
	{
		float l = sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2) + powf(w, 2));
		return Vector4(x / l, y / l, z / l, w / l);
	}

	void Vector4::Absolutize()
	{
		x = fabsf(x);
		y = fabsf(y);
		z = fabsf(z);
		w = fabsf(w);
	}

	Vector4 Vector4::GetAbsolutized() const
	{
		return Vector4(fabsf(x), fabsf(y), fabsf(z), fabsf(w));
	}

	void Vector4::ToArray(float* array) const
	{
		array[0] = x;
		array[1] = y;
		array[2] = z;
		array[3] = w;
	}

	std::string Vector4::ToString() const
	{
		return "Vector4(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
	}

	float Vector4::GetMagnitude() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float Vector4::GetMagnitudeSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	void Vector4::Clear()
	{
		x = y = z = w = 0.0f;
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

	bool Vector4::operator== (const Vector4& v) const
	{
		return Math::Approximately(x, v.x) && Math::Approximately(y, v.y) && Math::Approximately(z, v.z) && Math::Approximately(w, v.w);
	}

	bool Vector4::operator!= (const Vector4& v) const
	{
		return !Math::Approximately(x, v.x) || !Math::Approximately(y, v.y) || !Math::Approximately(z, v.z) || !Math::Approximately(w, v.w);
	}

	Vector4& Vector4::operator= (const Vector4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	Vector4& Vector4::operator= (const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	Vector4 Vector4::operator- () const
	{
		return Vector4(-x, -y, -z, -w);
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

	Vector4& Vector4::operator+= (const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	Vector4& Vector4::operator-= (const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	Vector4& Vector4::operator*= (const Vector4& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
	}

	Vector4& Vector4::operator/= (const Vector4& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
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

	Vector4& Vector4::operator+= (float n)
	{
		x += n;
		y += n;
		z += n;
		w += n;
		return *this;
	}

	Vector4& Vector4::operator-= (float n)
	{
		x -= n;
		y -= n;
		z -= n;
		w -= n;
		return *this;
	}

	Vector4& Vector4::operator*= (float n)
	{
		x *= n;
		y *= n;
		z *= n;
		w *= n;
		return *this;
	}

	Vector4& Vector4::operator/= (float n)
	{
		x /= n;
		y /= n;
		z /= n;
		w /= n;
		return *this;
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

	Vector4 Vector4::operator*= (const Matrix4& mat)
	{
		float tmp_x = x * mat.m00 + y * mat.m10 + z * mat.m20 + w * mat.m30;
		float tmp_y = x * mat.m01 + y * mat.m11 + z * mat.m21 + w * mat.m31;
		float tmp_z = x * mat.m02 + y * mat.m12 + z * mat.m22 + w * mat.m32;
		float tmp_w = x * mat.m03 + y * mat.m13 + z * mat.m23 + w * mat.m33;

		x = tmp_x;
		y = tmp_y;
		z = tmp_z;
		w = tmp_w;

		return *this;
	}
}