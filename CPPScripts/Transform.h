#pragma once
#include "Component.h"

namespace ZXEngine
{
	class Transform : public Component
	{
	public:
		Vector3 position = Vector3(0, 0, 0);
		Quaternion rotation = Quaternion();
		Vector3 scale = Vector3(1, 1, 1);

		Transform() {};
		~Transform() {};

		static ComponentType GetType();

		Matrix4 GetModelMatrix();
		Matrix4 GetPositionMatrix();
		Matrix4 GetRotationMatrix();
		Matrix4 GetScaleMatrix();

		Vector3 GetForward();
		Vector3 GetRight();
		Vector3 GetUp();
	};
}