#pragma once
#include <string>
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	class GameObject
	{
	public:
		GameObject();
		~GameObject();

		Component* GetComponent(std::string type);

	private:
		Transform* transform;
		MeshRenderer* meshRenderer;
	};
}