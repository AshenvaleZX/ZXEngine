#pragma once
#include "WindowManagerMacOS.h"
#include "../ProjectSetting.h"

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

namespace ZXEngine
{
	WindowManagerMacOS::WindowManagerMacOS()
	{
		CGRect frame;
		frame.origin.x = 0.0;
		frame.origin.y = 0.0;
		frame.size.width = static_cast<CGFloat>(ProjectSetting::srcWidth);
		frame.size.height = static_cast<CGFloat>(ProjectSetting::srcHeight);

		NS::Window* wnd = NS::Window::alloc()->init(frame,
			NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
			NS::BackingStoreBuffered, false
		);

		wnd->setTitle(NS::String::string("ZXEngine <Metal>", NS::StringEncoding::UTF8StringEncoding));
		// 让窗口出现在最上层
		wnd->makeKeyAndOrderFront(nullptr);

		mWindow = wnd;
	}

	void* WindowManagerMacOS::GetWindow()
	{
		return mWindow;
	}

	void WindowManagerMacOS::CloseWindow(string args)
	{
		static_cast<NS::Window*>(mWindow)->close();
	}

	bool WindowManagerMacOS::WindowShouldClose()
	{
		return false;
	}
}