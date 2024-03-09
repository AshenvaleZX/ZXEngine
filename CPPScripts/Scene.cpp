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
#include "SceneManager.h"
#include "GameLogicManager.h"
#include "ParticleSystemManager.h"
#include "Audio/AudioEngine.h"

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

		// 临时切换SceneManager里的当前场景为this，因为GameObject在构造的时候可能会需要用到当前这个场景
		auto tmpScene = new SceneInfo();
		tmpScene->scene = this;
		auto curScene = SceneManager::GetInstance()->curScene;
		SceneManager::GetInstance()->curScene = tmpScene;

		for (auto prefab : sceneStruct->prefabs)
		{
			auto gameObject = new GameObject(prefab);
			gameObjects.push_back(gameObject);
		}

		for (auto gameObject : gameObjects)
		{
			gameObject->EndConstruction();
			mPhyScene->AddGameObject(gameObject);
		}

		ProjectSetting::renderPipelineType = curPipelineType;
		SceneManager::GetInstance()->curScene = curScene;
		delete tmpScene;
	}

	Scene::~Scene()
	{
		delete skyBox;

		for (auto gameObject : gameObjects)
		{
			delete gameObject;
		}

		Resources::ClearAsyncLoad();
	}

	void Scene::Update()
	{
		if (!mIsAwake)
		{
			for (auto gameObject : gameObjects)
			{
				gameObject->Awake();
			}
			mIsAwake = true;
		}

		GameLogicManager::GetInstance()->Update();
		Animator::Update();
		UpdatePhysics();
		AudioEngine::GetInstance()->Update();
		ParticleSystemManager::GetInstance()->Update();
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

	void Scene::AddGameObject(GameObject* gameObject)
	{
		gameObjects.push_back(gameObject);
	}

	void Scene::UpdatePhysics()
	{
		if (ProjectSetting::stablePhysics)
		{
			long long targetFrame = Time::curTime_micro / Time::fixedDeltaTime_micro;
			long long deltaFrame = targetFrame - mCurPhyFrame;

			// 当前游戏帧数高于物理引擎的目标帧数，跳过这一帧的物理更新
			if (deltaFrame <= 0)
				return;
			// 当前游戏帧数低于物理引擎的目标帧数，需要补物理帧，但是不能补太多，否则会导致游戏帧率进一步降低，恶性循环然后卡死
			else if (deltaFrame > 10)
				deltaFrame = 10;

			for (long long i = 0; i < deltaFrame; i++)
			{
				mPhyScene->BeginFrame();
				GameLogicManager::GetInstance()->FixedUpdate();
				mPhyScene->Update(Time::fixedDeltaTime);
				mPhyScene->EndFrame();
			}

			// 无论有没有补帧，或者补了多少帧，都当作已经追上了目标帧数
			mCurPhyFrame = targetFrame;
		}
		else
		{
			mPhyScene->BeginFrame();
			GameLogicManager::GetInstance()->FixedUpdate();
			mPhyScene->Update(Time::deltaTime);
			mPhyScene->EndFrame();
		}
	}
}