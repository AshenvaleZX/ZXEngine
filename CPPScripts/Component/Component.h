#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class GameObject;
	class Transform;
	class Component
	{
	public:
		static ComponentType GetType();

	public:
		GameObject* gameObject = nullptr;

		Component() {};
		~Component() {};

		virtual ComponentType GetInsType() = 0;

		Transform* GetTransform();

	private:
		
	};
}