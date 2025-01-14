#pragma once
#include <string>
#include <cstdint>

namespace ZXEngine
{
	class Vector3;
	class Matrix4;
	class Matrix3
	{
		friend class Math;
		friend class Vector3;
		friend class Matrix4;

	public:
		static const Matrix3 Zero;
		static const Matrix3 Identity;

		static Matrix3 CreateRotation(const Vector3& axis, float angle);

	public:
		Matrix3();
		Matrix3(float n);
		Matrix3(const Matrix3& mat3);
		Matrix3(const Matrix4& mat4);
		Matrix3(const Vector3& v1, const Vector3& v2, const Vector3& v3);
		Matrix3(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22
		);
		// 斜对称矩阵构造函数
		Matrix3(const Vector3& v);
		// 旋转矩阵构造函数(绕axis轴旋转angle度)
		Matrix3(const Vector3& axis, float angle);

		float GetNorm() const;
		float GetDeterminant() const;
		float Get(uint32_t row, uint32_t col) const;
		Vector3 GetRow(uint32_t index) const;
		Vector3 GetColumn(uint32_t index) const;

		void Transpose();

		void ToRowMajorArray(float* array) const;
		void ToColumnMajorArray(float* array) const;

		std::string ToString() const;

		Matrix3& operator= (const Matrix3& mat);
		bool operator== (const Matrix3& mat) const;
		bool operator!= (const Matrix3& mat) const;
		Matrix3 operator- () const;
		Matrix3 operator* (float n) const;
		Matrix3 operator+ (const Matrix3& mat) const;
		Matrix3 operator- (const Matrix3& mat) const;
		Matrix3 operator* (const Matrix3& mat) const;
		Matrix3& operator*= (float n);
		Matrix3& operator+= (const Matrix3& mat);
		Matrix3& operator-= (const Matrix3& mat);
		Matrix3& operator*= (const Matrix3& mat);
		Vector3 operator* (const Vector3& v) const;
		friend Matrix3 operator* (float n, const Matrix3& mat);

	private:
		union
		{
			float m[3][3];
			struct
			{
				// 第一行
				float m00; float m01; float m02;
				// 第二行
				float m10; float m11; float m12;
				// 第三行
				float m20; float m21; float m22;
			};
		};
	};
}