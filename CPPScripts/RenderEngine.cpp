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

#ifdef ZX_EDITOR
#include "Editor/EditorCamera.h"
#include "Editor/EditorDataManager.h"
#endif

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

	void RenderEngine::Render()
	{
		SceneManager::GetInstance()->GetCurScene()->RenderPrepare();

#ifdef ZX_EDITOR
		auto camera = EditorDataManager::GetInstance()->isGameView ? Camera::GetMainCamera() : EditorCamera::GetInstance()->mCamera;
#else
		auto camera = Camera::GetMainCamera();
#endif

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