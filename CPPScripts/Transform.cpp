#include "Transform.h"
#include "GameObject.h"

namespace ZXEngine
{
	ComponentType Transform::GetType()
	{
		return ComponentType::Transform;
	}

	ComponentType Transform::GetInsType()
	{
		return ComponentType::Transform;
	}


	Matrix4 Transform::GetModelMatrix() const
	{
		return GetPositionMatrix() * GetRotationMatrix() * GetScaleMatrix();
	}

	Matrix4 Transform::GetPositionMatrix() const
	{
		Vector3 position = GetPosition();
		return Matrix4(
			1, 0, 0, position.x,
			0, 1, 0, position.y,
			0, 0, 1, position.z,
			0, 0, 0, 1);
	}

	Matrix4 Transform::GetRotationMatrix() const
	{
		Quaternion rotation = GetRotation();
		return rotation.ToMatrix();
	}

	Matrix4 Transform::GetScaleMatrix() const
	{
		Vector3 scale = GetScale();
		return Matrix4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1);
	}

	Vector3 Transform::GetLocalScale() const
	{
		return localScale;
	}

	Vector3 Transform::GetLocalPosition() const
	{
		return localPosition;
	}

	Vector3 Transform::GetLocalEulerAngles() const
	{
		return localRotation.GetEulerAngles();
	}

	Quaternion Transform::GetLocalRotation() const
	{
		return localRotation;
	}

	void Transform::SetLocalScale(const Vector3& scale)
	{
		localScale = scale;
	}

	void Transform::SetLocalScale(float x, float y, float z)
	{
		SetLocalScale(Vector3(x, y, z));
	}

	void Transform::SetLocalPosition(const Vector3& position)
	{
		localPosition = position;
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(Vector3(x, y, z));
	}

	void Transform::SetLocalEulerAngles(const Vector3& eulerAngles)
	{
		localRotation.SetEulerAngles(eulerAngles);
	}

	void Transform::SetLocalEulerAngles(float x, float y, float z)
	{
		SetLocalEulerAngles(Vector3(x, y, z));
	}

	void Transform::SetLocalRotation(const Quaternion& rotation)
	{
		localRotation = rotation;
	}

	Vector3 Transform::GetScale() const
	{
		return GetLocalScale();
	}

	Vector3 Transform::GetPosition() const
	{
		return GetLocalPosition();
	}

	Vector3 Transform::GetEulerAngles() const
	{
		return GetLocalEulerAngles();
	}

	Quaternion Transform::GetRotation() const
	{
		return GetLocalRotation();
	}

	void Transform::SetScale(const Vector3& scale)
	{
		SetLocalScale(scale);
	}

	void Transform::SetScale(float x, float y, float z)
	{
		SetScale(Vector3(x, y, z));
	}

	void Transform::SetPosition(const Vector3& position)
	{
		SetLocalPosition(position);
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(Vector3(x, y, z));
	}

	void Transform::SetEulerAngles(const Vector3& eulerAngles)
	{
		SetLocalEulerAngles(eulerAngles);
	}

	void Transform::SetEulerAngles(float x, float y, float z)
	{
		SetEulerAngles(Vector3(x, y, z));
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		SetLocalRotation(rotation);
	}

	Vector3 Transform::GetUp() const
	{
		Quaternion rotation = GetRotation();
		Vector4 up = rotation.ToMatrix() * Vector4(0, 1, 0, 0);
		return Vector3(up.x, up.y, up.z);
	}

	Vector3 Transform::GetRight() const
	{
		Quaternion rotation = GetRotation();
		Vector4 right = rotation.ToMatrix() * Vector4(1, 0, 0, 0);
		return Vector3(right.x, right.y, right.z);
	}

	Vector3 Transform::GetForward() const
	{
		Quaternion rotation = GetRotation();
		Vector4 forward = rotation.ToMatrix() * Vector4(0, 0, 1, 0);
		return Vector3(forward.x, forward.y, forward.z);
	}
}