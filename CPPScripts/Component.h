#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class GameObject;
	class Transform;
	class Component
	{
	public:
		GameObject* gameObject = nullptr;

		Component() {};
		~Component() {};

		static ComponentType GetType();

		Transform* GetTransform();

	private:
		
	};
}