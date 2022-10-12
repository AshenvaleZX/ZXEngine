#include "RenderEngine.h"

namespace ZXEngine
{
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

	void RenderEngine::Render(Camera* camera, GameObject* gameObject)
	{
		Debug::Log("Render");
	}
}