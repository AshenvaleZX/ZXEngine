#include "Scene.h"
#include "RenderEngine.h"
#include "RenderQueueManager.h"

namespace ZXEngine
{
	Scene::Scene(SceneStruct* sceneStruct)
	{
		for (auto cam : sceneStruct->cameras)
		{
			cameras.push_back(new Camera());
		}

		for (auto prefab : sceneStruct->prefabs)
		{
			gameObjects.push_back(new GameObject());
		}
	}
	
	void Scene::Render()
	{
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