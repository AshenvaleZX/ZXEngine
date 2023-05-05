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
		// ���ڴ�С�仯
		case WM_SIZE:
			mWindowWidth = static_cast<uint32_t>(LOWORD(lParam));
			mWindowHeight = static_cast<uint32_t>(HIWORD(lParam));

			// ������С��
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			// �������
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			// ���ڴ�С�ָ�
			else if (wParam == SIZE_RESTORED)
			{
				// ����С���ָ�
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}
				// ����󻯻ָ�
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				// �û������϶����ڱ߿�
				else if (mResizing)
				{
					// ��ʱ���ڴ�С��һֱ��ͣ�仯��������Ӧ�����������ٴ���
					// Ҳ�����յ� WM_EXITSIZEMOVE ��ʱ��
				}
				// һЩ����������µĴ��ڱ仯������ SetWindowPos �� mSwapChain->SetFullscreenState �Ƚӿڵĵ���
				else
				{
					OnResize();
				}
			}
			return 0;

		// �û����ϴ��ڱ߿�
		case WM_ENTERSIZEMOVE:
			mAppPaused = true;
			mResizing = true;
			return 0;

		// �û��ɿ��˴��ڱ߿�
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