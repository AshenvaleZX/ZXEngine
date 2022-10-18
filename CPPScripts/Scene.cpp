#include "Scene.h"
#include "RenderEngine.h"
#include "RenderQueueManager.h"

namespace ZXEngine
{
	void Scene::Render()
	{
		if (cameras.size() == 0)
			cameras.push_back(new Camera());

		if (gameObjects.size() == 0)
			gameObjects.push_back(new GameObject());

		for (unsigned i = 0; i < cameras.size(); ++i) 
		{
			auto camera = cameras[i];
			for (unsigned j = 0; j < gameObjects.size(); ++j)
			{
				auto gameObject = gameObjects[j];
				RenderQueueManager::GetInstance()->AddRenderer(gameObject->GetComponent<MeshRenderer>("MeshRenderer"));
			}
			RenderEngine::GetInstance()->Render(camera);
		}
	}
}