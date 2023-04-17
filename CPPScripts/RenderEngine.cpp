#include "RenderEngine.h"
#include "EventManager.h"
#include "MeshRenderer.h"
#include "PublicEnum.h"
#include "SceneManager.h"
#include "CubeMap.h"
#include "ZCamera.h"
#include "RenderPassManager.h"
#include "RenderPass.h"
#include "RenderQueueManager.h"
#include "FBOManager.h"
#include "ParticleSystemManager.h"
#include "ProjectSetting.h"
#include "RenderEngineProperties.h"
#include "Window/WindowManager.h"

namespace ZXEngine
{
	RenderEngine* RenderEngine::mInstance = nullptr;

	void RenderEngine::Create()
	{
		mInstance = new RenderEngine();
		RenderEngineProperties::Create();
		RenderQueueManager::Creat();
		RenderAPI::Creat();
		FBOManager::Create();
		ParticleSystemManager::Create();
		RenderPassManager::Create();
	}

	RenderEngine* RenderEngine::GetInstance()
	{
		return mInstance;
	}

	RenderEngine::RenderEngine()
	{
		WindowManager::Creat();

		EventManager::GetInstance()->AddEventHandler((int)EventType::KEY_ESCAPE_PRESS, std::bind(&RenderEngine::CloseWindow, this, std::placeholders::_1));
	}

	void RenderEngine::CloseWindow(string args)
	{
		WindowManager::GetInstance()->CloseWindow(args);
	}

	int RenderEngine::WindowShouldClose()
	{
		return WindowManager::GetInstance()->WindowShouldClose();
	}

	void RenderEngine::BeginRender()
	{
		RenderAPI::GetInstance()->BeginFrame();
	}

	void RenderEngine::Render(Camera* camera)
	{
		auto renderPassMgr = RenderPassManager::GetInstance();
		for (unsigned int i = 0; i < renderPassMgr->passes.size(); i++)
		{
			renderPassMgr->passes[i]->Render(camera);
		}
	}

	void RenderEngine::EndRender()
	{
		RenderAPI::GetInstance()->EndFrame();
	}
}