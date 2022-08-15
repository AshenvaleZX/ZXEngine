#include "Game.h"

namespace ZXEngine
{
	const unsigned int SCR_WIDTH = 1280;
	const unsigned int SCR_HEIGHT = 720;

	SceneManager* SceneManager::mInstance;

	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);

	void Game::Play()
	{
		SceneManager::mInstance = new SceneManager();
		InitWindow();
		Debug::Log("ZXTest");

		while (!glfwWindowShouldClose(window))
		{
			Render();
		}
	}

	void Game::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ZXEngine", NULL, NULL);
		if (window == NULL)
		{
			cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	}

	void Game::Render()
	{
		SceneManager::mInstance->GetCurScene()->Render();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}
}