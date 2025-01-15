#pragma once
#include "WindowManager.h"

namespace ZXEngine
{
	class WindowManagerAndroid : public WindowManager
	{
	public:
		WindowManagerAndroid() = default;
		~WindowManagerAndroid() = default;

		virtual void* GetWindow();
		virtual void CloseWindow(string args);
		virtual bool WindowShouldClose();

	private:
		android_app* mApp = nullptr;
	};
}