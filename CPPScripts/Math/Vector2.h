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
		float& r = x;
		float& g = y;

		Vector2();
		Vector2(float n);
		Vector2(float x, float y);
		Vector2(const Vector2& v);
		Vector2(const Vector3& v);
		Vector2(const Vector4& v);

		void ToArray(float* array);
		std::string ToString();

		float& operator[] (int i);
		const float& operator[] (int i) const;
		Vector2& operator= (const Vector2& v);
		Vector2 operator+ (const Vector2& v) const;
		Vector2 operator- (const Vector2& v) const;
		Vector2 operator* (const Vector2& v) const;
		Vector2 operator/ (const Vector2& v) const;
		Vector2 operator+= (const Vector2& v) const;
		Vector2 operator-= (const Vector2& v) const;
		Vector2 operator*= (const Vector2& v) const;
		Vector2 operator/= (const Vector2& v) const;
		Vector2 operator+ (float n) const;
		Vector2 operator- (float n) const;
		Vector2 operator* (float n) const;
		Vector2 operator/ (float n) const;
		Vector2 operator+= (float n) const;
		Vector2 operator-= (float n) const;
		Vector2 operator*= (float n) const;
		Vector2 operator/= (float n) const;
		friend Vector2 operator+ (float n, const Vector2& v);
		friend Vector2 operator- (float n, const Vector2& v);
		friend Vector2 operator* (float n, const Vector2& v);
		friend Vector2 operator/ (float n, const Vector2& v);
	};
}