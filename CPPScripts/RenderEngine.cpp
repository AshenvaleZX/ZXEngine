#include "RenderEngine.h"
#include "RenderQueueManager.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	RenderEngine* RenderEngine::GetInstance()
	{
		return mInstance;
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}

	GLFWwindow* RenderEngine::window;
	unsigned int RenderEngine::scrWidth;
	unsigned int RenderEngine::scrHeight;
	void RenderEngine::InitWindow(unsigned int width, unsigned int height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		scrWidth = width;
		scrHeight = height;
		window = glfwCreateWindow(scrWidth, scrHeight, "ZXEngine", NULL, NULL);
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
	}

	void RenderEngine::SwapBufferAndPollPollEvents()
	{
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	int RenderEngine::WindowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void RenderEngine::Render(Camera* camera)
	{
		Debug::Log("Render");

		auto renderQueue = RenderQueueManager::GetInstance()->GetRenderQueue(RenderQueueType::Qpaque);

		mat4 mat_V = camera->GetViewMatrix();
		mat4 mat_P = camera->GetProjectionMatrix();
		for (auto renderer : renderQueue->GetRenderers())
		{
			Debug::Log("Shader ID " + to_string(renderer->matetrial->shader->GetID()));
			Debug::Log("Mesh num " + to_string(renderer->meshes.size()));
		}

		// 每次渲染完要清空，下次要渲染的时候再重新添加
		RenderQueueManager::GetInstance()->ClearAllRenderQueue();
	}
}