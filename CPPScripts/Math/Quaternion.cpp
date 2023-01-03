#include "Quaternion.h"
#include "../Math.h"

namespace ZXEngine
{
	Quaternion Quaternion::Euler(float x, float y, float z)
	{
		// 参考: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		float pitch = Math::Deg2Rad(x);
		float yaw = Math::Deg2Rad(y);
		float roll = Math::Deg2Rad(z);
		float cp = (float)cos(pitch * 0.5);
		float sp = (float)sin(pitch * 0.5);
		float cy = (float)cos(yaw * 0.5);
		float sy = (float)sin(yaw * 0.5);
		float cr = (float)cos(roll * 0.5);
		float sr = (float)sin(roll * 0.5);

		Quaternion q;
		q.x = cr * sp * cy + sr * cp * sy;
		q.y = cr * cp * sy - sr * sp * cy;
		q.z = sr * cp * cy - cr * sp * sy;
		q.w = cr * cp * cy + sr * sp * sy;

		return q;
	}

	Quaternion Quaternion::Euler(const Vector3& eulerAngles)
	{
		return Euler(eulerAngles.x, eulerAngles.y , eulerAngles.z);
	}

	Quaternion::Quaternion() 
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1.0f;
	}

	Quaternion::Quaternion(const Quaternion& q)
	{
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
	}

	Quaternion::Quaternion(float x, float y, float z, float w) 
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Quaternion::Normalize()
	{
		float sqrNorm = MagnitudeSquare();
		if (!Math::Approximately(sqrNorm, 0.f))
		{
			float normInverse = 1.0f / sqrtf(sqrNorm);
			x *= normInverse;
			y *= normInverse;
			z *= normInverse;
			w *= normInverse;
		}
	}

	Quaternion Quaternion::GetInverse() const
	{
		// 参考: https://www.mathworks.com/help/aeroblks/quaternioninverse.html
		// 四元数的逆操作按理说应该有一个除以长度的计算，但是我们这里表达旋转的四元数长度都是1，所以直接省略了这一步
		return Quaternion(-x, -y, -z, w);
	}

	Vector3 Quaternion::GetEulerAngles() const
	{
		// 参考: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		float sinp = sqrt(1 + 2 * (w * x - y * z));
		float cosp = sqrt(1 - 2 * (w * x - y * z));
		float pitch = 2 * atan2(sinp, cosp) - Math::PI / 2.0f;

		float siny_cosp = 2 * (w * y + z * x);
		float cosy_cosp = 1 - 2 * (x * x + y * y);
		float yaw = atan2(siny_cosp, cosy_cosp);

		float sinr_cosp = 2 * (w * z + x * y);
		float cosr_cosp = 1 - 2 * (z * z + x * x);
		float roll = atan2(sinr_cosp, cosr_cosp);

		Vector3 angles;
		angles.x = Math::Rad2Deg(pitch);
		angles.y = Math::Rad2Deg(yaw);
		angles.z = Math::Rad2Deg(roll);
		return angles;
	}

	void Quaternion::SetEulerAngles(float x, float y, float z) 
	{
		Quaternion q = Quaternion::Euler(x, y ,z);
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
	}

	void Quaternion::SetEulerAngles(const Vector3& eulerAngles)
	{
		SetEulerAngles(eulerAngles.x, eulerAngles.y, eulerAngles.z);
	}

	Matrix4 Quaternion::ToMatrix() const
	{
		// 第一行
		float m00 = 1 - (2 * y * y) - (2 * z * z);
		float m01 = (2 * x * y) - (2 * w * z);
		float m02 = (2 * x * z) + (2 * w * y);

		// 第二行
		float m10 = (2 * x * y) + (2 * w * z);
		float m11 = 1 - (2 * x * x) - (2 * z * z);
		float m12 = (2 * y * z) - (2 * w * x);

		// 第三行
		float m20 = (2 * x * z) - (2 * w * y);
		float m21 = (2 * y * z) + (2 * w * x);
		float m22 = 1 - (2 * x * x) - (2 * y * y);

		return Matrix4(
			m00, m01, m02, 0,
			m10, m11, m12, 0,
			m20, m21, m22, 0,
			0, 0, 0, 1);
	}

	Quaternion Quaternion::operator* (const Quaternion& q) const
	{
		// 参考: https://www.mathworks.com/help/aeroblks/quaternionmultiplication.html
		float qx =  q.x * w + q.y * z - q.z * y + q.w * x;
		float qy = -q.x * z + q.y * w + q.z * x + q.w * y;
		float qz =  q.x * y - q.y * x + q.z * w + q.w * z;
		float qw = -q.x * x - q.y * y - q.z * z + q.w * w;
		Quaternion result = Quaternion(qx, qy, qz, qw);
		result.Normalize();
		return result;
	}

	Quaternion& Quaternion::operator*= (const Quaternion& q)
	{
		// 把this解引用再乘
		*this = *this * q;
		return *this;
	}

	float Quaternion::Magnitude() const
	{
		return sqrtf(MagnitudeSquare());
	}

	float Quaternion::MagnitudeSquare() const
	{
		return x * x + y * y + z * z + w * w;
	}
}