#pragma once
#include <string>

namespace ZXEngine
{
	class Vector3;
	class Vector4;
	class Vector2
	{
	public:
		float x;
		float y;

		Vector2();
		Vector2(float n);
		Vector2(float x, float y);
		Vector2(const Vector2& v);
		Vector2(const Vector3& v);
		Vector2(const Vector4& v);

		std::string ToString();

		Vector2 operator+ (const Vector2& v) const;
		Vector2 operator- (const Vector2& v) const;
		Vector2 operator* (const Vector2& v) const;
		Vector2 operator/ (const Vector2& v) const;
		Vector2 operator+ (float n) const;
		Vector2 operator- (float n) const;
		Vector2 operator* (float n) const;
		Vector2 operator/ (float n) const;
	};
}