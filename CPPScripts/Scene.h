#pragma once
#include "pubh.h"
#include "GameObject.h"

namespace ZXEngine
{
	class Scene
	{
	public:
		Scene();
		~Scene();

	private:
		list<GameObject*> gameObjects;
	};
}