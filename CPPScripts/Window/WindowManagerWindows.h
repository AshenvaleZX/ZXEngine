#pragma once
#include "WindowManager.h"

// ��ֹwindows.h��ĺ궨��max��minӰ�쵽�����������ͬ�ֶ�
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

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

		bool mResized = false;
		bool mResizing = false;
		bool mMinimized = false;
		bool mMaximized = false;
		bool mAppPaused = false;

		void OnResize();
		void OnMouseScroll(short delta);
	};
}