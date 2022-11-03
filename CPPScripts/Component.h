#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class GameObject;
	class Component
	{
	public:
		GameObject* gameObject = nullptr;

		Component() {};
		~Component() {};

	private:
		
	};
}