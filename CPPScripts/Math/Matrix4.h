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
		Matrix4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33
		);

		float GetDeterminant() const;
		void ToRowMajorArray(float* array) const;
		void ToColumnMajorArray(float* array) const;
		std::string ToString();

		Vector4 operator* (const Vector4& v) const;
		Matrix4 operator+ (const Matrix4& mat) const;
		Matrix4 operator- (const Matrix4& mat) const;
		Matrix4 operator* (const Matrix4& mat) const;

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