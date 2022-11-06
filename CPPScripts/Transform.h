#pragma once
#include "Component.h"
#include "Quaternion.h"

namespace ZXEngine
{
	class Transform : public Component
	{
	public:
		vec3 position = vec3(0, 0, 0);
		Quaternion rotation = Quaternion();
		vec3 scale = vec3(1, 1, 1);

		Transform() {};
		~Transform() {};

		vec3 GetForward();
		vec3 GetRight();
		vec3 GetUp();
	private:

	};
}