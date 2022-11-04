#include "Scene.h"
#include "RenderEngine.h"
#include "RenderQueueManager.h"

namespace ZXEngine
{
	Scene::Scene(SceneStruct* sceneStruct)
	{
		for (auto prefab : sceneStruct->prefabs)
		{
			gameObjects.push_back(new GameObject(prefab));
		}
	}
	
	void Scene::Render()
	{
		for (unsigned i = 0; i < Camera::GetAllCameras().size(); ++i)
		{
			auto camera = Camera::GetAllCameras()[i];
			for (unsigned j = 0; j < gameObjects.size(); ++j)
			{
				auto gameObject = gameObjects[j];
				MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>("MeshRenderer");
				if (meshRenderer != nullptr)
				{
					RenderQueueManager::GetInstance()->AddRenderer(gameObject->GetComponent<MeshRenderer>("MeshRenderer"));
				}
			}
			RenderEngine::GetInstance()->Render(camera);
		}
	}
}