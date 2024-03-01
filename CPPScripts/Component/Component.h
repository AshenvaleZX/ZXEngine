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
		static bool IsUnique(ComponentType type);

	public:
		GameObject* gameObject = nullptr;

		Component() {};
		~Component() {};

		virtual void Awake();
		virtual ComponentType GetInsType() = 0;

		Transform* GetTransform();

	protected:
		bool mIsAwake = false;
	};
}