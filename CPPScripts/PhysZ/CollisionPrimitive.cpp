#include "CollisionPrimitive.h"
#include "RigidBody.h"

namespace ZXEngine
{
	namespace PhysZ
	{
		CollisionPrimitive::~CollisionPrimitive()
		{
			if (mRigidBody->mCollisionVolume == this)
			{
				mRigidBody->mCollisionVolume = nullptr;
			}
		}

		ColliderType CollisionBox::GetType() const
		{
			return ColliderType::Box;
		}

		Matrix3 CollisionBox::GetInertiaTensor(float mass) const
		{
			// 由于是halfSize，所以要先乘2再平方，简写为*4
			float x2 = mHalfSize.x * mHalfSize.x * 4.0f;
			float y2 = mHalfSize.y * mHalfSize.y * 4.0f;
			float z2 = mHalfSize.z * mHalfSize.z * 4.0f;

			return Matrix3(
				// 1/12 * m * (y^2 + z^2)
				0.083333f * mass * (y2 + z2), 0.0f, 0.0f,
				// 1/12 * m * (x^2 + z^2)
				0.0f, 0.083333f * mass * (x2 + z2), 0.0f,
				// 1/12 * m * (x^2 + y^2)
				0.0f, 0.0f, 0.083333f * mass * (x2 + y2));
		}

		float CollisionBox::GetHalfProjectedLength(const Vector3& axis) const
		{
			// 先把轴从世界空间变换到Box的局部空间(仅考虑旋转即可)
			Vector3 axisProjected = Math::Transpose(Matrix3(mTransform)) * axis;
			// 和mHalfSize放到同一个象限内
			axisProjected.Absolutize();
			// 然后计算Box在这个轴上的投影长度
			return Math::Dot(mHalfSize, axisProjected);
		}

		ColliderType CollisionPlane::GetType() const
		{
			return ColliderType::Plane;
		}

		Matrix3 CollisionPlane::GetInertiaTensor(float mass) const
		{
			// 无限远平面不当作刚体来模拟，惯性张量无意义
			return Matrix3();
		}

		ColliderType CollisionSphere::GetType() const
		{
			return ColliderType::Sphere;
		}

		Matrix3 CollisionSphere::GetInertiaTensor(float mass) const
		{
			// 2/5 * m * r^2
			float i = 0.4f * mass * mRadius * mRadius;

			return Matrix3(
				i, 0.0f, 0.0f,
				0.0f, i, 0.0f,
				0.0f, 0.0f, i);
		}
	}
}