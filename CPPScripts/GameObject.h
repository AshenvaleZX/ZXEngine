#pragma once
#include "pubh.h"
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

		Component* GetComponent(string type);

	private:
		Transform* transform;
		MeshRenderer* meshRenderer;
	};
}