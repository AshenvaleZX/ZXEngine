#pragma once
#include "WindowManager.h"
#include <GLFW/glfw3.h>

namespace ZXEngine
{
	class WindowManagerGLFW : public WindowManager
	{
	public:
		WindowManagerGLFW();
		~WindowManagerGLFW() {};

		virtual void* GetWindow();
		virtual void CloseWindow(string args);
		virtual bool WindowShouldClose();

	private:
		GLFWwindow* mWindow;
	};
}