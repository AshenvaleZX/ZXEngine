#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace ZXEngine
{
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

	std::string Vector2::ToString()
	{
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
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
}