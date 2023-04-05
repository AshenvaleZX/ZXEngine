#pragma once
#include <string>

namespace ZXEngine
{
	class Matrix3
	{
		friend class Math;
		friend class Vector3;
		friend class Matrix4;
	public:
		Matrix3();
		Matrix3(float n);
		Matrix3(const Matrix3& mat3);
		Matrix3(const Matrix4& mat4);
		Matrix3(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22
		);

		float GetDeterminant() const;
		void ToRowMajorArray(float* array) const;
		void ToColumnMajorArray(float* array) const;
		std::string ToString();

		bool operator== (const Matrix3& v) const;
		bool operator!= (const Matrix3& v) const;
		Vector3 operator* (const Vector3& v) const;
		Matrix3 operator+ (const Matrix3& mat) const;
		Matrix3 operator- (const Matrix3& mat) const;
		Matrix3 operator* (const Matrix3& mat) const;

	private:
		// 第一行
		float m00; float m01; float m02;
		// 第二行
		float m10; float m11; float m12;
		// 第三行
		float m20; float m21; float m22;
	};
}