#pragma once
#include "pubh.h"
#include "GameObject.h"
#include "ZCamera.h"
#include "Resources.h"

namespace ZXEngine
{
	class Scene
	{
	public:
		Scene(SceneStruct* sceneStruct);
		~Scene() {};

		void Render();

	private:
		vector<GameObject*> gameObjects;
		vector<Camera*> cameras;
	};
}