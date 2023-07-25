#include "Transform.h"
#include "../GameObject.h"

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

	Matrix4 Transform::GetLocalPositionMatrix() const
	{
		return Matrix4(
			1, 0, 0, localPosition.x,
			0, 1, 0, localPosition.y,
			0, 0, 1, localPosition.z,
			0, 0, 0, 1);
	}

	Matrix4 Transform::GetLocalRotationMatrix() const
	{
		return localRotation.ToMatrix();
	}

	Matrix4 Transform::GetLocalScaleMatrix() const
	{
		return Matrix4(
			localScale.x, 0,            0,            0,
			0,            localScale.y, 0,            0,
			0,            0,            localScale.z, 0,
			0,            0,            0,            1);
	}

	Matrix4 Transform::GetModelMatrix() const
	{
		// �Ѷ����Local Space�任��World Space�ľ���
		return GetPositionMatrix() * GetRotationAndScaleMatrix();
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
		// ��TransformǶ����������������ת������£�scaleֻ�ܵ��Ƴ����������������
		auto invRotationMat = Math::Inverse(GetRotation().ToMatrix());
		auto rotationAndScaleMat = GetRotationAndScaleMatrix();
		return invRotationMat * rotationAndScaleMat;
	}

	Matrix4 Transform::GetRotationAndScaleMatrix() const
	{
		auto localScaleMat = GetLocalScaleMatrix();
		auto localRotationMat = GetLocalRotationMatrix();
		if (gameObject->parent == nullptr)
		{
			return localRotationMat * localScaleMat;
		}
		else
		{
			return gameObject->parent->GetComponent<Transform>()->GetRotationAndScaleMatrix() * localRotationMat * localScaleMat;
		}
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

	Matrix3 Transform::GetScale() const
	{
		// ��TransformǶ����������������תʱ��scale�޷����ü򵥵�Vector3����
		// ��Ϊ��������ʱ���Ӷ����������ת��������Ӷ�����˵���Ų����Ǳ�׼��xyz��������
		// ��Ӧ�ģ�����Ҳ�޷�����Vector3���ֻ����Matrix3����
		return Matrix3(GetScaleMatrix());
	}

	Vector3 Transform::GetPosition() const
	{
		if (gameObject->parent == nullptr)
		{
			return localPosition;
		}
		else
		{
			auto parent = gameObject->parent->GetComponent<Transform>();
			Vector3 offset = parent->GetRotationAndScaleMatrix() * localPosition;
			return parent->GetPosition() + offset;
		}
	}

	Vector3 Transform::GetEulerAngles() const
	{
		return GetRotation().GetEulerAngles();
	}

	Quaternion Transform::GetRotation() const
	{
		if (gameObject->parent == nullptr)
			return localRotation;
		else
			return localRotation * gameObject->parent->GetComponent<Transform>()->GetRotation();
	}

	void Transform::SetPosition(const Vector3& position)
	{
		if (gameObject->parent == nullptr)
		{
			localPosition = position;
		}
		else
		{
			auto wPosition = GetPosition();
			auto parent = gameObject->parent->GetComponent<Transform>();
			Vector3 offset = parent->GetRotationAndScaleMatrix() * localPosition;
			localPosition = Math::Inverse(parent->GetRotationAndScaleMatrix()) * (offset + position - wPosition);
		}
	}

	void Transform::SetPosition(float x, float y, float z)
	{
		SetPosition(Vector3(x, y, z));
	}

	void Transform::SetEulerAngles(const Vector3& eulerAngles)
	{
		SetRotation(Quaternion::Euler(eulerAngles));
	}

	void Transform::SetEulerAngles(float x, float y, float z)
	{
		SetEulerAngles(Vector3(x, y, z));
	}

	void Transform::SetRotation(const Quaternion& rotation)
	{
		if (gameObject->parent == nullptr)
			localRotation = rotation;
		else
			localRotation = gameObject->parent->GetComponent<Transform>()->GetRotation().GetInverse() * rotation;
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