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

	Scene::~Scene()
	{
		delete skyBox;

		for (auto gameObject : gameObjects)
		{
			delete gameObject;
		}
	}
	
	void Scene::Render()
	{
		for (unsigned i = 0; i < Camera::GetAllCameras().size(); ++i)
		{
			auto camera = Camera::GetAllCameras()[i];
			if (camera->cameraType != CameraType::GameCamera)
				continue;

			for (auto gameObject : gameObjects)
			{
				RenderQueueManager::GetInstance()->AddGameObject(gameObject);
			}

			RenderEngine::GetInstance()->Render(camera);
		}
	}
}