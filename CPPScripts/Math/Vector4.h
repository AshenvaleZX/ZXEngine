#pragma once
#include <string>
#include "../Reflection/StaticReflection.h"

namespace ZXEngine
{
	class Vector2;
	class Vector3;
	class Matrix4;
	class Vector4
	{
	public:
		static const Vector4 Zero;
		static const Vector4 One;

	public:
		union
		{
			struct { float v[4]; };
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
		};

		Vector4();
		Vector4(float n);
		Vector4(float x, float y, float z, float w);
		Vector4(const Vector2& v2);
		Vector4(const Vector2& v2l, const Vector2& v2r);
		Vector4(const Vector2& v2, float z, float w);
		Vector4(float x, const Vector2& v2, float w);
		Vector4(float x, float y, const Vector2& v2);
		Vector4(const Vector3& v3);
		Vector4(const Vector3& v3, float w);
		Vector4(float x, const Vector3& v3);
		Vector4(const Vector4& v);

		void Normalize();
		Vector4 GetNormalized() const;

		void Absolutize();
		Vector4 GetAbsolutized() const;
		
		float GetMagnitude() const;
		float GetMagnitudeSquared() const;
		
		void ToArray(float* array) const;
		std::string ToString() const;
		void Clear();

		float& operator[] (int i);
		const float& operator[] (int i) const;
		bool operator== (const Vector4& v) const;
		bool operator!= (const Vector4& v) const;
		Vector4& operator= (const Vector4& v);
		Vector4& operator= (const Vector3& v);
		Vector4 operator- () const;
		Vector4 operator+ (const Vector4& v) const;
		Vector4 operator- (const Vector4& v) const;
		Vector4 operator* (const Vector4& v) const;
		Vector4 operator/ (const Vector4& v) const;
		Vector4& operator+= (const Vector4& v);
		Vector4& operator-= (const Vector4& v);
		Vector4& operator*= (const Vector4& v);
		Vector4& operator/= (const Vector4& v);
		Vector4 operator+ (float n) const;
		Vector4 operator- (float n) const;
		Vector4 operator* (float n) const;
		Vector4 operator/ (float n) const;
		Vector4& operator+= (float n);
		Vector4& operator-= (float n);
		Vector4& operator*= (float n);
		Vector4& operator/= (float n);
		friend Vector4 operator+ (float n, const Vector4& v);
		friend Vector4 operator- (float n, const Vector4& v);
		friend Vector4 operator* (float n, const Vector4& v);
		friend Vector4 operator/ (float n, const Vector4& v);
		Vector4 operator* (const Matrix4& mat) const;
		Vector4 operator*= (const Matrix4& mat);
	};

	ZXRef_StaticReflection
	(
		Vector4,
		ZXRef_Fields
		(
			ZXRef_Field(&Vector4::x),
			ZXRef_Field(&Vector4::y),
			ZXRef_Field(&Vector4::z),
			ZXRef_Field(&Vector4::w)
		)
	)
}