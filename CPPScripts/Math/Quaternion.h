#pragma once

namespace ZXEngine
{
	class Vector3;
	class Matrix4;
	class Quaternion
	{
	public:
		static Quaternion Euler(float x, float y, float z);
		static Quaternion Euler(const Vector3& eulerAngles);

	public:
		union { float x, i; };
		union { float y, j; };
		union { float z, k; };
		union { float w, r; };

		Quaternion();
		Quaternion(const Quaternion& q);
		Quaternion(const Vector3& v, float w);
		Quaternion(float x, float y, float z, float w);
		~Quaternion() {};

		void Normalize();
		Quaternion GetInverse() const;

		Vector3 GetEulerAngles() const;
		void SetEulerAngles(float x, float y, float z);
		void SetEulerAngles(const Vector3& eulerAngles);

		/// <summary>
		/// 绕axis轴旋转angle度
		/// </summary>
		/// <param name="axis">旋转轴(必须是单位向量)</param>
		/// <param name="angle">旋转角度(弧度制)</param>
		void Rotate(const Vector3& axis, float angle);
		void RotateByVector(const Vector3& rotation, float scale = 1.0f);

		Matrix4 ToMatrix() const;

		bool operator== (const Quaternion& q) const;
		bool operator!= (const Quaternion& q) const;
		Quaternion& operator= (const Quaternion& q);
		Quaternion operator* (const Quaternion& q) const;
		Quaternion& operator*= (const Quaternion& q);

	private:
		float Magnitude() const;
		float MagnitudeSquare() const;
	};
}
