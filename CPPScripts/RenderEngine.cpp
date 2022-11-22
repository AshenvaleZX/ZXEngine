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

namespace ZXEngine
{
	RenderEngine* RenderEngine::mInstance = nullptr;

	void RenderEngine::Create()
	{
		mInstance = new RenderEngine();
	}

	RenderEngine* RenderEngine::GetInstance()
	{
		return mInstance;
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		GlobalData::srcWidth = width;
		GlobalData::srcHeight = height;
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		RenderAPI::GetInstance()->SetWindowSize(width, height);
	}

	void RenderEngine::InitWindow(unsigned int width, unsigned int height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GlobalData::srcWidth = width;
		GlobalData::srcHeight = height;
		window = glfwCreateWindow(width, height, "ZXEngine", NULL, NULL);
		if (window == NULL)
		{
			Debug::LogError("Failed to create GLFW window");
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
		
		EventManager::GetInstance()->AddEventHandler(EventType::KEY_ESCAPE_PRESS, std::bind(&RenderEngine::CloseWindow, this, std::placeholders::_1));
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
		SwapBufferAndPollPollEvents();
	}
}