#pragma once

namespace ZXEngine
{
	class Matrix4
	{
		friend class Math;
		friend class Vector4;
		friend class Matrix3;
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

		float GetDeterminant() const;
		Vector4 GetRow(uint32_t index) const;
		Vector4 GetColumn(uint32_t index) const;

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
		// 第一行
		float m00; float m01; float m02; float m03;
		// 第二行
		float m10; float m11; float m12; float m13;
		// 第三行
		float m20; float m21; float m22; float m23;
		// 第四行
		float m30; float m31; float m32; float m33;
	};
}