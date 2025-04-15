#pragma once
#include "WindowManager.h"

#define NS_PRIVATE_IMPLEMENTATION
#include <AppKit/AppKit.hpp>

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
        NS::Window* mWindow;
	};
}