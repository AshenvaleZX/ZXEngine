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
		InitWindow();
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		RenderAPI::GetInstance()->OnWindowSizeChange(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

	void RenderEngine::InitWindow()
	{
		glfwInit();

#ifdef ZX_API_OPENGL
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#ifdef ZX_API_VULKAN
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

		window = glfwCreateWindow(ProjectSetting::srcWidth, ProjectSetting::srcHeight, "ZXEngine", NULL, NULL);
		if (window == NULL)
		{
			Debug::LogError("Failed to create GLFW window");
			glfwTerminate();
			return;
		}

#ifdef ZX_API_OPENGL
		glfwMakeContextCurrent(window);
#endif

		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

		EventManager::GetInstance()->AddEventHandler((int)EventType::KEY_ESCAPE_PRESS, std::bind(&RenderEngine::CloseWindow, this, std::placeholders::_1));
	}

	void RenderEngine::CloseWindow(string args)
	{
		glfwSetWindowShouldClose(window, true);
	}

	void RenderEngine::SwapBufferAndPollPollEvents()
	{
#ifdef ZX_API_OPENGL
		glfwSwapBuffers(window);
#endif
		glfwPollEvents();
	}

	int RenderEngine::WindowShouldClose()
	{
		return glfwWindowShouldClose(window);
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
		// 交换前后缓冲区，poll事件
		SwapBufferAndPollPollEvents();
	}
}