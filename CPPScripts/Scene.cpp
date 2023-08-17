#include "Scene.h"
#include "RenderEngine.h"
#include "RenderQueueManager.h"
#include "CubeMap.h"
#include "GameObject.h"
#include "Component/ZCamera.h"
#include "Resources.h"
#include "ProjectSetting.h"
#include "RenderAPI.h"
#include "PhysZ/PhysZ.h"
#include "Time.h"

namespace ZXEngine
{
	Scene::Scene(SceneStruct* sceneStruct)
	{
		mPhyScene = new PhysZ::PScene(1000, 1000);
		skyBox = new CubeMap(sceneStruct->skyBox);
		renderPipelineType = sceneStruct->renderPipelineType;

		if (renderPipelineType == RenderPipelineType::RayTracing)
			rtPipelineID = RenderAPI::GetInstance()->CreateRayTracingPipeline(sceneStruct->rtShaderPathGroup);

		// 临时切换渲染管线类型，因为用于光追场景的模型需要生成BLAS，但是目前会先加载场景再切换场景
		// 所以这里先切换到当前加载的场景对应的管线，加载完场景再切换回去
		auto curPipelineType = ProjectSetting::renderPipelineType;
		ProjectSetting::renderPipelineType = renderPipelineType;

		for (auto prefab : sceneStruct->prefabs)
		{
			auto gameObject = new GameObject(prefab);
			gameObjects.push_back(gameObject);
			mPhyScene->AddGameObject(gameObject);
		}

		ProjectSetting::renderPipelineType = curPipelineType;
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

	void Scene::UpdatePhysics()
	{
		mPhyScene->BeginFrame();
		mPhyScene->Update(Time::deltaTime);
		mPhyScene->EndFrame();
	}
}