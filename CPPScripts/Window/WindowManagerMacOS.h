#pragma once
#include "WindowManager.h"

namespace ZXEngine
{
	class WindowManagerMacOS : public WindowManager
	{
	public:
		WindowManagerMacOS();
		~WindowManagerMacOS() {};

		virtual void* GetWindow();
		virtual void CloseWindow(string args);
		virtual bool WindowShouldClose();

	private:
		void* mWindow;
	};
}