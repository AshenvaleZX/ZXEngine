#pragma once
#include "WindowManagerMacOS.h"
#include "../ProjectSetting.h"

namespace ZXEngine
{
	WindowManagerMacOS::WindowManagerMacOS()
	{
		CGRect frame;
		frame.origin.x = 0.0;
		frame.origin.y = 0.0;
		frame.size.width = static_cast<CGFloat>(ProjectSetting::srcWidth);
		frame.size.height = static_cast<CGFloat>(ProjectSetting::srcHeight);

		mWindow = NS::Window::alloc()->init(frame,
			NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
			NS::BackingStoreBuffered, false
		);

		mWindow->setTitle(NS::String::string("ZXEngine <Metal>", NS::StringEncoding::UTF8StringEncoding));
		// 让窗口出现在最上层
		mWindow->makeKeyAndOrderFront(nullptr);
	}

	void* WindowManagerMacOS::GetWindow()
	{
		return static_cast<void*>(mWindow);
	}

	void WindowManagerMacOS::CloseWindow(string args)
	{
		mWindow->close();
	}

	bool WindowManagerMacOS::WindowShouldClose()
	{
		return false;
	}
}