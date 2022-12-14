#include "Transform.h"

namespace ZXEngine
{
	ComponentType Transform::GetType()
	{
		return ComponentType::T_Transform;
	}

	ComponentType Transform::GetInsType()
	{
		return ComponentType::T_Transform;
	}

	Matrix4 Transform::GetModelMatrix()
	{
		return GetPositionMatrix() * GetRotationMatrix() * GetScaleMatrix();
	}

	Vector3 Transform::GetForward()
	{
		Vector4 forward = rotation.ToMatrix() * Vector4(0, 0, 1, 0);
		return Vector3(forward.x, forward.y, forward.z);
	}

	Vector3 Transform::GetRight()
	{
		Vector4 right = rotation.ToMatrix() * Vector4(1, 0, 0, 0);
		return Vector3(right.x, right.y, right.z);
	}

	Vector3 Transform::GetUp()
	{
		Vector4 up = rotation.ToMatrix() * Vector4(0, 1, 0, 0);
		return Vector3(up.x, up.y, up.z);
	}

	Matrix4 Transform::GetPositionMatrix()
	{
		return Matrix4(
			1, 0, 0, position.x,
			0, 1, 0, position.y,
			0, 0, 1, position.z,
			0, 0, 0, 1);
	}

	Matrix4 Transform::GetRotationMatrix()
	{
		return rotation.ToMatrix();
	}

	Matrix4 Transform::GetScaleMatrix()
	{
		return Matrix4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1);
	}
}