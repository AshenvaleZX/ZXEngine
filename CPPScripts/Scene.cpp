#include "Scene.h"

namespace ZXEngine
{
	void Scene::Render()
	{
		for (auto i = 0; i < cameras.size(); ++i) {
			auto camera = cameras[i];
			camera->Render(gameObjects);
		}
	}
}