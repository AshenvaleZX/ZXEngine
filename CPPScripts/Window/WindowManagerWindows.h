#pragma once
#include "WindowManager.h"

namespace ZXEngine
{
	class WindowManagerWindows : public WindowManager
	{
	public:
		WindowManagerWindows();
		~WindowManagerWindows() {};

		virtual void* GetWindow();
		virtual void CloseWindow(string args);
		virtual bool WindowShouldClose();

	private:
		HWND mWindow;
	};
}