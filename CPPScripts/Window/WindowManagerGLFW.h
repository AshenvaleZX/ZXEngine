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

		float GetWindowScaleX() const { return mWindowScaleX; }
		float GetWindowScaleY() const { return mWindowScaleY; }

	private:
		GLFWwindow* mWindow;

		float mWindowScaleX = 1.0f;
		float mWindowScaleY = 1.0f;
	};
}