#include "WindowManagerGLFW.h"
#include "../ProjectSetting.h"
#include "../RenderAPI.h"
#include "../GlobalData.h"

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
#ifdef ZX_PLATFORM_MACOS
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

		// 在macOS上使用WindowSize回调，WindowSize是窗口的逻辑分辨率大小，FramebufferSize是窗口的物理分辨率大小
		// 如果直接使用物理分辨率大小，在系统分辨率设置有缩放的情况下，渲染输出分辨率和实际窗口分辨率会不匹配
#ifdef ZX_PLATFORM_MACOS
		glfwSetWindowSizeCallback(mWindow, FrameBufferSizeCallback);
#else
		glfwSetFramebufferSizeCallback(mWindow, FrameBufferSizeCallback);
#endif

		int realWindowWidth, realWindowHeight;
#ifdef ZX_PLATFORM_MACOS
		glfwGetWindowSize(mWindow, &realWindowWidth, &realWindowHeight);
#else
		glfwGetFramebufferSize(mWindow, &realWindowWidth, &realWindowHeight);
#endif

		// 预期创建的窗口大小和最终实际创建的窗口大小如果不一致的话，就重新设置一下
		uint32_t realWindowWidth_ui32 = static_cast<uint32_t>(realWindowWidth);
		uint32_t realWindowHeight_ui32 = static_cast<uint32_t>(realWindowHeight);
		if (realWindowWidth_ui32 != ProjectSetting::srcWidth || realWindowHeight_ui32 != ProjectSetting::srcHeight)
		{
#ifdef ZX_EDITOR
			ProjectSetting::SetWindowSize(realWindowWidth_ui32, realWindowHeight_ui32);
#else
			GlobalData::srcWidth = realWindowWidth_ui32;
			GlobalData::srcHeight = realWindowHeight_ui32;
			ProjectSetting::SetWindowSize();
#endif
		}
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