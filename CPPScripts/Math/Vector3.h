#pragma once
#include <string>

namespace ZXEngine
{
	class Vector2;
	class Vector4;
	class Matrix3;
	class Vector3
	{
	public:
		float x;
		float y;
		float z;
		float& r = x;
		float& g = y;
		float& b = z;

		Vector3();
		Vector3(float n);
		Vector3(float x, float y, float z);
		Vector3(const Vector2& v2);
		Vector3(const Vector2& v2, float z);
		Vector3(const Vector3& v);
		Vector3(const Vector4& v);

		void ToArray(float* array);
		std::string ToString();

		float& operator[] (int i);
		const float& operator[] (int i) const;
		Vector3& operator= (const Vector3& v);
		Vector3 operator+ (const Vector3& v) const;
		Vector3 operator- (const Vector3& v) const;
		Vector3 operator* (const Vector3& v) const;
		Vector3 operator/ (const Vector3& v) const;
		Vector3 operator+= (const Vector3& v) const;
		Vector3 operator-= (const Vector3& v) const;
		Vector3 operator*= (const Vector3& v) const;
		Vector3 operator/= (const Vector3& v) const;
		Vector3 operator+ (float n) const;
		Vector3 operator- (float n) const;
		Vector3 operator* (float n) const;
		Vector3 operator/ (float n) const;
		Vector3 operator+= (float n) const;
		Vector3 operator-= (float n) const;
		Vector3 operator*= (float n) const;
		Vector3 operator/= (float n) const;
		friend Vector3 operator+ (float n, const Vector3& v);
		friend Vector3 operator- (float n, const Vector3& v);
		friend Vector3 operator* (float n, const Vector3& v);
		friend Vector3 operator/ (float n, const Vector3& v);
		Vector3 operator* (const Matrix3& mat) const;
	};
}