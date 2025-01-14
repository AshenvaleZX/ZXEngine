#pragma once
#include <cstdint>

namespace ZXEngine
{
	class Vector4;
	class Matrix3;
	class Matrix4
	{
		friend class Math;
		friend class Vector4;
		friend class Matrix3;

	public:
		static const Matrix4 Zero;
		static const Matrix4 Identity;

		static Matrix4 CreateTranslation(const Vector3& v);
		static Matrix4 CreateRotation(const Vector3& axis, float angle);
		static Matrix4 CreateScale(const Vector3& scale);

	public:
		Matrix4();
		Matrix4(float n);
		Matrix4(const Matrix3& mat3);
		Matrix4(const Matrix4& mat4);
		Matrix4(const Vector4& v0, const Vector4& v1, const Vector4& v2, const Vector4& v3);
		Matrix4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		);

		float GetNorm() const;
		float GetDeterminant() const;
		float Get(uint32_t row, uint32_t col) const;
		Vector4 GetRow(uint32_t index) const;
		Vector4 GetColumn(uint32_t index) const;

		void Transpose();
		void Translate(const Vector3& v);
		void Rotate(float angle, const Vector3& axis);
		void Rotate(const Vector3& from, const Vector3& to);
		void Scale(const Vector3& scale);

		void ToRowMajorArray(float* array) const;
		void ToColumnMajorArray(float* array) const;

		std::string ToString() const;

		Matrix4& operator= (const Matrix4& mat);
		bool operator== (const Matrix4& mat) const;
		bool operator!= (const Matrix4& mat) const;
		Matrix4 operator- () const;
		Matrix4 operator* (float n) const;
		Matrix4 operator+ (const Matrix4& mat) const;
		Matrix4 operator- (const Matrix4& mat) const;
		Matrix4 operator* (const Matrix4& mat) const;
		Matrix4& operator*= (float n);
		Matrix4& operator+= (const Matrix4& mat);
		Matrix4& operator-= (const Matrix4& mat);
		Matrix4& operator*= (const Matrix4& mat);
		Vector4 operator* (const Vector4& v) const;
		friend Matrix4 operator* (float n, const Matrix4& mat);

	private:
		union
		{
			float m[4][4];
			struct
			{
				// 第一行
				float m00; float m01; float m02; float m03;
				// 第二行
				float m10; float m11; float m12; float m13;
				// 第三行
				float m20; float m21; float m22; float m23;
				// 第四行
				float m30; float m31; float m32; float m33;
			};
		};
	};
}