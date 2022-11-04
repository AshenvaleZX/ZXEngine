#pragma once
#include "Component.h"

namespace ZXEngine
{
	class Transform : public Component
	{
	public:
		vec3 position = vec3(0, 0, 0);
		vec3 rotation = vec3(0, 0, 0);
		vec3 scale = vec3(1, 1, 1);

		Transform() {};
		~Transform() {};

	private:

	};
}