#pragma once
#include <string>
#include "../Reflection/StaticReflection.h"

namespace ZXEngine
{
	class Vector3;
	class Vector4;
	class Vector2
	{
	public:
		static const Vector2 Zero;
		static const Vector2 One;
		static const Vector2 Up;
		static const Vector2 Down;
		static const Vector2 Left;
		static const Vector2 Right;

	public:
		union
		{
			struct { float v[2]; };
			struct { float x, y; };
			struct { float r, g; };
		};

		Vector2();
		Vector2(float n);
		Vector2(float x, float y);
		Vector2(const Vector2& v);
		Vector2(const Vector3& v);
		Vector2(const Vector4& v);

		void Normalize();
		Vector2 GetNormalized() const;

		void Absolutize();
		Vector2 GetAbsolutized() const;

		float GetMagnitude() const;
		float GetMagnitudeSquared() const;

		void ToArray(float* array) const;
		std::string ToString() const;
		void Clear();

		float& operator[] (int i);
		const float& operator[] (int i) const;
		bool operator== (const Vector2& v) const;
		bool operator!= (const Vector2& v) const;
		Vector2& operator= (const Vector2& v);
		Vector2 operator+ (const Vector2& v) const;
		Vector2 operator- (const Vector2& v) const;
		Vector2 operator* (const Vector2& v) const;
		Vector2 operator/ (const Vector2& v) const;
		Vector2& operator+= (const Vector2& v);
		Vector2& operator-= (const Vector2& v);
		Vector2& operator*= (const Vector2& v);
		Vector2& operator/= (const Vector2& v);
		Vector2 operator+ (float n) const;
		Vector2 operator- (float n) const;
		Vector2 operator* (float n) const;
		Vector2 operator/ (float n) const;
		Vector2& operator+= (float n);
		Vector2& operator-= (float n);
		Vector2& operator*= (float n);
		Vector2& operator/= (float n);
		friend Vector2 operator+ (float n, const Vector2& v);
		friend Vector2 operator- (float n, const Vector2& v);
		friend Vector2 operator* (float n, const Vector2& v);
		friend Vector2 operator/ (float n, const Vector2& v);
	};

	ZXRef_StaticReflection
	(
		Vector2,
		ZXRef_Fields
		(
			ZXRef_Field(&Vector2::x),
			ZXRef_Field(&Vector2::y)
		)
	)
}