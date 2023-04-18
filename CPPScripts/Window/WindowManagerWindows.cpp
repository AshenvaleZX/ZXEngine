#include "WindowManagerWindows.h"
#include <WindowsX.h>
#include "../ProjectSetting.h"
#include "../Input/InputManager.h"

namespace ZXEngine
{
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WindowManagerWindows::WindowManagerWindows()
	{
		WNDCLASSEXW wndClass = {};
		wndClass.cbSize = sizeof(wndClass);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = GetModuleHandle(NULL);
		wndClass.hIcon = LoadIcon(0, IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(0, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wndClass.lpszMenuName = 0;
		wndClass.lpszClassName = L"MainWnd";
		wndClass.hIconSm = LoadIcon(0, IDI_APPLICATION);

		RegisterClassExW(&wndClass);

		mWindow = CreateWindowW(wndClass.lpszClassName, L"ZXEngine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			ProjectSetting::srcWidth, ProjectSetting::srcHeight, NULL, NULL, wndClass.hInstance, NULL);

		ShowWindow(mWindow, SW_SHOW);
		UpdateWindow(mWindow);
	}

	void* WindowManagerWindows::GetWindow()
	{
		return static_cast<void*>(mWindow);
	}

	void WindowManagerWindows::CloseWindow(string args)
	{
		
	}

	bool WindowManagerWindows::WindowShouldClose()
	{
		return false;
	}

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_MOUSEMOVE:
			InputManager::GetInstance()->UpdateMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}