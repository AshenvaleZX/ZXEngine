#include "RenderEngine.h"
#include "EventManager.h"
#include "Component/MeshRenderer.h"
#include "PublicEnum.h"
#include "SceneManager.h"
#include "CubeMap.h"
#include "Component/ZCamera.h"
#include "RenderPassManager.h"
#include "RenderPass.h"
#include "RenderQueueManager.h"
#include "FBOManager.h"
#include "ParticleSystem/ParticleSystemManager.h"
#include "ProjectSetting.h"
#include "RenderEngineProperties.h"
#include "Window/WindowManager.h"

namespace ZXEngine
{
	RenderEngine* RenderEngine::mInstance = nullptr;

	void RenderEngine::Create()
	{
		mInstance = new RenderEngine();
		RenderQueueManager::Creat();
		RenderAPI::Creat();
		RenderEngineProperties::Create();
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
	}

	void RenderEngine::CloseWindow(const string& args)
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
		for (auto pass : renderPassMgr->curPasses)
		{
			pass->Render(camera);
		}
	}

	void RenderEngine::EndRender()
	{
		RenderAPI::GetInstance()->EndFrame();
	}
}