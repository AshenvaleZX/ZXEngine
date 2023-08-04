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
		float x;
		float y;
		float z;
		float w;

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

		void RotateByVector(const Vector3& rotation, float scale = 1.0f);

		Matrix4 ToMatrix() const;

		bool operator== (const Quaternion& q) const;
		bool operator!= (const Quaternion& q) const;
		Quaternion operator* (const Quaternion& q) const;
		Quaternion& operator*= (const Quaternion& q);

	private:
		float Magnitude() const;
		float MagnitudeSquare() const;
	};
}
