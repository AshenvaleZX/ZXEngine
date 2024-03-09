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

		// ��ʱ�л���Ⱦ�������ͣ���Ϊ���ڹ�׷������ģ����Ҫ����BLAS������Ŀǰ���ȼ��س������л�����
		// �����������л�����ǰ���صĳ�����Ӧ�Ĺ��ߣ������곡�����л���ȥ
		auto curPipelineType = ProjectSetting::renderPipelineType;
		ProjectSetting::renderPipelineType = renderPipelineType;

		// ��ʱ�л�SceneManager��ĵ�ǰ����Ϊthis����ΪGameObject�ڹ����ʱ����ܻ���Ҫ�õ���ǰ�������
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

			// ��ǰ��Ϸ֡���������������Ŀ��֡����������һ֡���������
			if (deltaFrame <= 0)
				return;
			// ��ǰ��Ϸ֡���������������Ŀ��֡������Ҫ������֡�����ǲ��ܲ�̫�࣬����ᵼ����Ϸ֡�ʽ�һ�����ͣ�����ѭ��Ȼ����
			else if (deltaFrame > 10)
				deltaFrame = 10;

			for (long long i = 0; i < deltaFrame; i++)
			{
				mPhyScene->BeginFrame();
				GameLogicManager::GetInstance()->FixedUpdate();
				mPhyScene->Update(Time::fixedDeltaTime);
				mPhyScene->EndFrame();
			}

			// ������û�в�֡�����߲��˶���֡���������Ѿ�׷����Ŀ��֡��
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