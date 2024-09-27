#pragma once
#include <string>
#include "../Reflection/StaticReflection.h"

namespace ZXEngine
{
	class Vector2;
	class Vector4;
	class Matrix3;
	class Vector3
	{
	public:
		static const Vector3 Zero;
		static const Vector3 One;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Left;
		static const Vector3 Right;
		static const Vector3 Forward;
		static const Vector3 Back;

	public:
		union { float x, r; };
		union { float y, g; };
		union { float z, b; };

		Vector3();
		Vector3(float n);
		Vector3(float x, float y, float z);
		Vector3(const Vector2& v2);
		Vector3(const Vector2& v2, float z);
		Vector3(float x, const Vector2& v2);
		Vector3(const Vector3& v);
		Vector3(const Vector4& v);

		void Normalize();
		Vector3 GetNormalized() const;

		void Absolutize();
		Vector3 GetAbsolutized() const;

		float GetMagnitude() const;
		float GetMagnitudeSquared() const;

		void ToArray(float* array) const;
		std::string ToString() const;
		void Clear();

		Vector4 ToPosVec4() const;
		Vector4 ToDirVec4() const;

		float& operator[] (int i);
		const float& operator[] (int i) const;
		bool operator== (const Vector3& v) const;
		bool operator!= (const Vector3& v) const;
		Vector3& operator= (const Vector3& v);
		Vector3 operator- () const;
		Vector3 operator+ (const Vector3& v) const;
		Vector3 operator- (const Vector3& v) const;
		Vector3 operator* (const Vector3& v) const;
		Vector3 operator/ (const Vector3& v) const;
		Vector3& operator+= (const Vector3& v);
		Vector3& operator-= (const Vector3& v);
		Vector3& operator*= (const Vector3& v);
		Vector3& operator/= (const Vector3& v);
		Vector3 operator+ (float n) const;
		Vector3 operator- (float n) const;
		Vector3 operator* (float n) const;
		Vector3 operator/ (float n) const;
		Vector3& operator+= (float n);
		Vector3& operator-= (float n);
		Vector3& operator*= (float n);
		Vector3& operator/= (float n);
		friend Vector3 operator+ (float n, const Vector3& v);
		friend Vector3 operator- (float n, const Vector3& v);
		friend Vector3 operator* (float n, const Vector3& v);
		friend Vector3 operator/ (float n, const Vector3& v);
		Vector3 operator* (const Matrix3& mat) const;
		Vector3& operator*= (const Matrix3& mat);
	};

	StaticReflection
	(
		Vector3,
		Fields
		(
			Field(&Vector3::x),
			Field(&Vector3::y),
			Field(&Vector3::z)
		)
	)
}