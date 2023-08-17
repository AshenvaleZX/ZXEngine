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

		// ��ʱ�л���Ⱦ�������ͣ���Ϊ���ڹ�׷������ģ����Ҫ����BLAS������Ŀǰ���ȼ��س������л�����
		// �����������л�����ǰ���صĳ�����Ӧ�Ĺ��ߣ������곡�����л���ȥ
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