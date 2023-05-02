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

	public:
		void Show();
		LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		HWND mWindow;

		uint32_t mWindowWidth;
		uint32_t mWindowHeight;

		bool mResizing = false;
		bool mMinimized = false;
		bool mMaximized = false;
		bool mAppPaused = false;

		void OnResize();
	};
}