#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Camera;
	class CubeMap;
	class GameObject;
	struct SceneStruct;
	class Scene
	{
	public:
		CubeMap* skyBox = nullptr;

		Scene(SceneStruct* sceneStruct);
		~Scene() {};

		void Render();

	private:
		vector<GameObject*> gameObjects;
		vector<Camera*> cameras;
	};
}