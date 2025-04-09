#include "WindowManagerWindows.h"
#include <WindowsX.h>
#include "../RenderAPI.h"
#include "../ProjectSetting.h"
#include "../Input/InputManager.h"
#include "../Editor/EditorInputManager.h"
#include "../External/ImGui/imgui_impl_win32.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ZXEngine
{
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return static_cast<WindowManagerWindows*>(WindowManager::GetInstance())->WindowProcedure(hWnd, msg, wParam, lParam);
	}

	WindowManagerWindows::WindowManagerWindows()
	{
		// 处理系统缩放
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

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

		// CreateWindowW传入的窗口大小会包括标题栏和边框，需要根据客户区大小来计算实际窗口大小
		RECT rc = { 0, 0, static_cast<LONG>(mWindowWidth), static_cast<LONG>(mWindowHeight) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
		int width = static_cast<int>(rc.right - rc.left);
		int height = static_cast<int>(rc.bottom - rc.top);

		mWindow = CreateWindowW(wndClass.lpszClassName, L"ZXEngine <Direct3D 12>", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			width, height, NULL, NULL, wndClass.hInstance, NULL);
	}

	void* WindowManagerWindows::GetWindow()
	{
		return static_cast<void*>(mWindow);
	}

	void WindowManagerWindows::CloseWindow(string args)
	{
		PostQuitMessage(0);
	}

	bool WindowManagerWindows::WindowShouldClose()
	{
		if (mResized)
			OnResize();

		MSG msg;
		BOOL hasMsg;
		while (true)
		{
			hasMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);

			if (!hasMsg && !mAppPaused)
				break;

			if (!hasMsg)
				continue;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				return true;
		}

		return false;
	}

	LRESULT WindowManagerWindows::WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// 给ImGui传递Win32窗口消息
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		// 窗口大小变化
		case WM_SIZE:
			mWindowWidth = static_cast<uint32_t>(LOWORD(lParam));
			mWindowHeight = static_cast<uint32_t>(HIWORD(lParam));
			mResized = true;

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

		// 鼠标滚轮事件
		case WM_MOUSEWHEEL:
			OnMouseScroll(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;

		// 窗口关闭
		case WM_DESTROY:
			PostQuitMessage(0);
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
		{
			renderAPI->OnWindowSizeChange(mWindowWidth, mWindowHeight);
			mResized = false;
		}
	}

	void WindowManagerWindows::OnMouseScroll(short delta)
	{
		float deltaValue = static_cast<float>(delta) / 120.0f;
		InputManager::GetInstance()->UpdateMouseScroll(0.0f, deltaValue);
#ifdef ZX_EDITOR
		EditorInputManager::GetInstance()->UpdateMouseScroll(0.0f, deltaValue);
#endif
	}
}