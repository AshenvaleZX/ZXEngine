#include "WindowManager.h"

#if defined(ZX_PLATFORM_DESKTOP)
#	if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
#		include "WindowManagerGLFW.h"
#	elif defined(ZX_API_D3D12)
#		include "WindowManagerWindows.h"
#	elif defined(ZX_API_METAL)
#		include "WindowManagerMacOS.h"
#	endif
#elif defined(ZX_PLATFORM_ANDROID)
#	include "WindowManagerAndroid.h"
#endif

namespace ZXEngine
{
	WindowManager* WindowManager::mInstance = nullptr;

	void WindowManager::Creat()
	{
#if defined(ZX_PLATFORM_DESKTOP)
#	if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
		mInstance = new WindowManagerGLFW();
#	elif defined(ZX_API_D3D12)
		mInstance = new WindowManagerWindows();
		static_cast<WindowManagerWindows*>(mInstance)->Show();
#	elif defined(ZX_API_METAL)
		mInstance = new WindowManagerMacOS();
#	endif
#elif defined(ZX_PLATFORM_ANDROID)
		mInstance = new WindowManagerAndroid();
		static_cast<WindowManagerAndroid*>(mInstance)->Init();
#endif
	}

	WindowManager* WindowManager::GetInstance()
	{
		return mInstance;
	}
}