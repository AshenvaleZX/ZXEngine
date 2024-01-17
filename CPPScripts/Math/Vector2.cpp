#include <cassert>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "../Math.h"
#include "../Debug.h"

namespace ZXEngine
{
	const Vector2 Vector2::Zero  = Vector2( 0.0f,  0.0f);
	const Vector2 Vector2::One   = Vector2( 1.0f,  1.0f);
	const Vector2 Vector2::Up    = Vector2( 0.0f,  1.0f);
	const Vector2 Vector2::Down  = Vector2( 0.0f, -1.0f);
	const Vector2 Vector2::Left  = Vector2(-1.0f,  0.0f);
	const Vector2 Vector2::Right = Vector2( 1.0f,  0.0f);

	Vector2::Vector2()
	{
		this->x = 0.0f;
		this->y = 0.0f;
	}

	Vector2::Vector2(float n)
	{
		this->x = n;
		this->y = n;
	}

	Vector2::Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2::Vector2(const Vector2& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	Vector2::Vector2(const Vector3& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	Vector2::Vector2(const Vector4& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	void Vector2::Normalize()
	{
		float l = sqrtf(powf(x, 2) + powf(y, 2));
		x /= l;
		y /= l;
	}

	Vector2 Vector2::GetNormalized() const
	{
		float l = sqrtf(powf(x, 2) + powf(y, 2));
		return Vector2(x / l, y / l);
	}

	void Vector2::Absolutize()
	{
		x = fabsf(x);
		y = fabsf(y);
	}

	Vector2 Vector2::GetAbsolutized() const
	{
		return Vector2(fabsf(x), fabsf(y));
	}

	void Vector2::ToArray(float* array) const
	{
		array[0] = x;
		array[1] = y;
	}

	std::string Vector2::ToString() const
	{
		return "Vector2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}

	float Vector2::GetMagnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	float Vector2::GetMagnitudeSquared() const
	{
		return x * x + y * y;
	}

	void Vector2::Clear()
	{
		x = y = 0.0f;
	}

	float& Vector2::operator[] (int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			Debug::LogError("Invalid index to Vector2 !");
			assert(false);
			return x;
			break;
		}
	}

	const float& Vector2::operator[] (int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			Debug::LogError("Invalid index to Vector2 !");
			assert(false);
			return x;
			break;
		}
	}

	bool Vector2::operator== (const Vector2& v) const
	{
		return Math::Approximately(x, v.x) && Math::Approximately(y, v.y);
	}

	bool Vector2::operator!= (const Vector2& v) const
	{
		return !Math::Approximately(x, v.x) || !Math::Approximately(y, v.y);
	}

	Vector2& Vector2::operator= (const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	Vector2 Vector2::operator+ (const Vector2& v) const
	{
		return Vector2(x + v.x, y + v.y);
	}

	Vector2 Vector2::operator- (const Vector2& v) const
	{
		return Vector2(x - v.x, y - v.y);
	}

	Vector2 Vector2::operator* (const Vector2& v) const
	{
		return Vector2(x * v.x, y * v.y);
	}

	Vector2 Vector2::operator/ (const Vector2& v) const
	{
		return Vector2(x / v.x, y / v.y);
	}

	Vector2& Vector2::operator+= (const Vector2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vector2& Vector2::operator-= (const Vector2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vector2& Vector2::operator*= (const Vector2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vector2& Vector2::operator/= (const Vector2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vector2 Vector2::operator+ (float n) const
	{
		return Vector2(x + n, y + n);
	}

	Vector2 Vector2::operator- (float n) const
	{
		return Vector2(x - n, y - n);
	}

	Vector2 Vector2::operator* (float n) const
	{
		return Vector2(x * n, y * n);
	}

	Vector2 Vector2::operator/ (float n) const
	{
		return Vector2(x / n, y / n);
	}

	Vector2& Vector2::operator+= (float n)
	{
		x += n;
		y += n;
		return *this;
	}

	Vector2& Vector2::operator-= (float n)
	{
		x -= n;
		y -= n;
		return *this;
	}

	Vector2& Vector2::operator*= (float n)
	{
		x *= n;
		y *= n;
		return *this;
	}

	Vector2& Vector2::operator/= (float n)
	{
		x /= n;
		y /= n;
		return *this;
	}

	Vector2 operator+ (float n, const Vector2& v)
	{
		return Vector2(n + v.x, n + v.y);
	}

	Vector2 operator- (float n, const Vector2& v)
	{
		return Vector2(n - v.x, n - v.y);
	}

	Vector2 operator* (float n, const Vector2& v)
	{
		return Vector2(n * v.x, n * v.y);
	}

	Vector2 operator/ (float n, const Vector2& v)
	{
		return Vector2(n / v.x, n / v.y);
	}
}