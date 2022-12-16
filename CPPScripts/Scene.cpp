#include "Scene.h"
#include "RenderEngine.h"
#include "RenderQueueManager.h"
#include "CubeMap.h"
#include "GameObject.h"
#include "ZCamera.h"
#include "Resources.h"

namespace ZXEngine
{
	Scene::Scene(SceneStruct* sceneStruct)
	{
		skyBox = new CubeMap(sceneStruct->skyBox);

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
			if (camera->cameraType != CameraType::GameCamera)
				continue;

			for (unsigned j = 0; j < gameObjects.size(); ++j)
			{
				auto gameObject = gameObjects[j];

				if (gameObject->layer == GameObjectLayer::UI)
				{
					RenderQueueManager::GetInstance()->AddUIGameObject(gameObject);
				}
				else
				{
					MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
					if (meshRenderer != nullptr)
					{
						RenderQueueManager::GetInstance()->AddRenderer(gameObject->GetComponent<MeshRenderer>());
					}
				}
			}
			RenderEngine::GetInstance()->Render(camera);
		}
	}
}