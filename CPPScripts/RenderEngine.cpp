#include "RenderEngine.h"
#include "EventManager.h"
#include "MeshRenderer.h"
#include "PublicEnum.h"
#include "SceneManager.h"
#include "CubeMap.h"
#include "ZCamera.h"
#include "RenderPassManager.h"
#include "RenderPass.h"
#include "GlobalData.h"
#include "RenderQueueManager.h"
#include "FBOManager.h"
#include "ParticleSystemManager.h"
#include "ProjectSetting.h"
#include "RenderAPIVulkan.h"

namespace ZXEngine
{
	RenderEngine* RenderEngine::mInstance = nullptr;

	void RenderEngine::Create()
	{
		mInstance = new RenderEngine();
		RenderQueueManager::Creat();
		RenderAPI::Creat();
		RenderAPI::GetInstance()->InitRenderSetting();
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
#ifdef ZX_EDITOR
		unsigned int hWidth = (width - GlobalData::srcWidth) / 3;
		unsigned int iWidth = width - GlobalData::srcWidth - hWidth;
		unsigned int pHeight = height - GlobalData::srcHeight - ProjectSetting::mainBarHeight;
		ProjectSetting::SetWindowSize(hWidth, pHeight, iWidth);
#else
		GlobalData::srcWidth = width;
		GlobalData::srcHeight = height;
		RenderAPI::GetInstance()->SetViewPort(width, height);
#endif

#ifdef ZX_API_VULKAN
		auto api = reinterpret_cast<RenderAPIVulkan*>(glfwGetWindowUserPointer(window));
		api->windowResized = true;
#endif
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

#ifdef ZX_API_VULKAN
		glfwSetWindowUserPointer(window, RenderAPI::GetInstance());
#endif

		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
		
		EventManager::GetInstance()->AddEventHandler((int)EventType::KEY_ESCAPE_PRESS, std::bind(&RenderEngine::CloseWindow, this, std::placeholders::_1));
	}

	void RenderEngine::CloseWindow(string args)
	{
		glfwSetWindowShouldClose(window, true);
	}

	void RenderEngine::SwapBufferAndPollPollEvents()
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	int RenderEngine::WindowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void RenderEngine::BeginRender()
	{
		
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
		// OpenGL异常不会主动抛出，每帧结束检测一下
		RenderAPI::GetInstance()->CheckError();
		// 交换前后缓冲区，poll事件
		SwapBufferAndPollPollEvents();
	}
}