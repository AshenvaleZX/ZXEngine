#pragma once
#include "Component.h"

namespace ZXEngine
{
	class Transform : public Component
	{
	public:
		static ComponentType GetType();

	public:
		Transform() {};
		~Transform() {};

		virtual ComponentType GetInsType();

		void Translate(const Vector3& offset);

		// Local Space
		Matrix4 GetLocalPositionMatrix() const;
		Matrix4 GetLocalRotationMatrix() const;
		Matrix4 GetLocalScaleMatrix() const;

		// World Space
		Matrix4 GetModelMatrix() const;
		Matrix4 GetPositionMatrix() const;
		Matrix4 GetRotationMatrix() const;
		Matrix4 GetScaleMatrix() const;
		Matrix4 GetRotationAndScaleMatrix() const;

		// Local Space
		Vector3 GetLocalScale() const;
		Vector3 GetLocalPosition() const;
		Vector3 GetLocalEulerAngles() const;
		Quaternion GetLocalRotation() const;
		void SetLocalScale(const Vector3& scale);
		void SetLocalScale(float x, float y, float z);
		void SetLocalPosition(const Vector3& position);
		void SetLocalPosition(float x, float y, float z);
		void SetLocalEulerAngles(const Vector3& eulerAngles);
		void SetLocalEulerAngles(float x, float y, float z);
		void SetLocalRotation(const Quaternion& rotation);

		// World Space
		Matrix3 GetScale() const;
		Vector3 GetPosition() const;
		Vector3 GetEulerAngles() const;
		Quaternion GetRotation() const;
		void SetPosition(const Vector3& position);
		void SetPosition(float x, float y, float z);
		void SetEulerAngles(const Vector3& eulerAngles);
		void SetEulerAngles(float x, float y, float z);
		void SetRotation(const Quaternion& rotation);

		// World Space
		Vector3 GetUp() const;
		Vector3 GetRight() const;
		Vector3 GetForward() const;

	protected:
		Vector3 localPosition = Vector3();
		Quaternion localRotation = Quaternion();
		Vector3 localScale = Vector3(1.0f);

		void UpdateColliderTransform() const;
	};
}