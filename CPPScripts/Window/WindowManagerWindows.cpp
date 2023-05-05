#include "WindowManagerWindows.h"
#include <WindowsX.h>
#include "../RenderAPI.h"
#include "../ProjectSetting.h"

namespace ZXEngine
{
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return static_cast<WindowManagerWindows*>(WindowManager::GetInstance())->WindowProcedure(hWnd, msg, wParam, lParam);
	}

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

		mWindowWidth = ProjectSetting::srcWidth;
		mWindowHeight = ProjectSetting::srcHeight;
		mWindow = CreateWindowW(wndClass.lpszClassName, L"ZXEngine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			ProjectSetting::srcWidth, ProjectSetting::srcHeight, NULL, NULL, wndClass.hInstance, NULL);
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

	LRESULT WindowManagerWindows::WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		// 窗口大小变化
		case WM_SIZE:
			mWindowWidth = static_cast<uint32_t>(LOWORD(lParam));
			mWindowHeight = static_cast<uint32_t>(HIWORD(lParam));

			// 窗口最小化
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			// 窗口最大化
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			// 窗口大小恢复
			else if (wParam == SIZE_RESTORED)
			{
				// 从最小化恢复
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}
				// 从最大化恢复
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				// 用户正在拖动窗口边框
				else if (mResizing)
				{
					// 此时窗口大小会一直不停变化，不做响应，等拖完了再处理
					// 也就是收到 WM_EXITSIZEMOVE 的时候
				}
				// 一些其他情况导致的窗口变化，比如 SetWindowPos 或 mSwapChain->SetFullscreenState 等接口的调用
				else
				{
					OnResize();
				}
			}
			return 0;

		// 用户在拖窗口边框
		case WM_ENTERSIZEMOVE:
			mAppPaused = true;
			mResizing = true;
			return 0;

		// 用户松开了窗口边框
		case WM_EXITSIZEMOVE:
			mAppPaused = false;
			mResizing = false;
			OnResize();
			return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	void WindowManagerWindows::Show()
	{
		ShowWindow(mWindow, SW_SHOW);
		UpdateWindow(mWindow);
	}

	void WindowManagerWindows::OnResize()
	{
		auto renderAPI = RenderAPI::GetInstance();
		if (renderAPI)
			renderAPI->OnWindowSizeChange(mWindowWidth, mWindowHeight);
	}
}