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
		windowName = "ZXEngine <OpenGL " + 
			to_string(ProjectSetting::OpenGLVersionMajor) +"." + 
			to_string(ProjectSetting::OpenGLVersionMinor) + ">";

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ProjectSetting::OpenGLVersionMajor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ProjectSetting::OpenGLVersionMinor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

#ifdef ZX_API_VULKAN
#ifdef __APPLE__
		windowName = "ZXEngine <Vulkan 1.2>";
#else
		windowName = "ZXEngine <Vulkan 1.3>";
#endif
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

		mWindow = glfwCreateWindow(ProjectSetting::srcWidth, ProjectSetting::srcHeight, windowName.c_str(), NULL, NULL);
		if (mWindow == NULL)
		{
			const char* description;
			int code = glfwGetError(&description);
			Debug::LogError("Failed to create GLFW window, error code: %s\nDescription: %s", code, description);
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