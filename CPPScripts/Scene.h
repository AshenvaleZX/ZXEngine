#pragma once
#include "pubh.h"
#include "GameObject.h"
#include "ZCamera.h"

namespace ZXEngine
{
	class Scene
	{
	public:
		Scene() {};
		~Scene() {};

		void Render();

	private:
		list<GameObject*> gameObjects;
		vector<Camera*> cameras;
	};
}