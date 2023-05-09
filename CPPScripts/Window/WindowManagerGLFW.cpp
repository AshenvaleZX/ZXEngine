#include "WindowManagerGLFW.h"
#include "../ProjectSetting.h"
#include "../RenderAPI.h"

namespace ZXEngine
{
	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		RenderAPI::GetInstance()->OnWindowSizeChange(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}

	WindowManagerGLFW::WindowManagerGLFW()
	{
		glfwInit();
		string windowName = "ZXEngine";

#ifdef ZX_API_OPENGL
		windowName = "ZXEngine <OpenGL 4.6>";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#ifdef ZX_API_VULKAN
		windowName = "ZXEngine <Vulkan 1.3>";
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

		mWindow = glfwCreateWindow(ProjectSetting::srcWidth, ProjectSetting::srcHeight, windowName.c_str(), NULL, NULL);
		if (mWindow == NULL)
		{
			Debug::LogError("Failed to create GLFW window");
			glfwTerminate();
			return;
		}

#ifdef ZX_API_OPENGL
		glfwMakeContextCurrent(mWindow);
#endif

		glfwSetFramebufferSizeCallback(mWindow, FrameBufferSizeCallback);
	}

	void* WindowManagerGLFW::GetWindow()
	{
		return static_cast<void*>(mWindow);
	}

	void WindowManagerGLFW::CloseWindow(string args)
	{
		glfwSetWindowShouldClose(mWindow, true);
	}

	bool WindowManagerGLFW::WindowShouldClose()
	{
		return glfwWindowShouldClose(mWindow) == 1;
	}
}