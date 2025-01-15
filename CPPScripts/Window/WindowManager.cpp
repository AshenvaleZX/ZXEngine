#include "WindowManager.h"

#if defined(ZX_PLATFORM_DESKTOP)
#	if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
#		include "WindowManagerGLFW.h"
#	elif defined(ZX_API_D3D12)
#		include "WindowManagerWindows.h"
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
#	endif
#elif defined(ZX_PLATFORM_ANDROID)
		mInstance = new WindowManagerAndroid();
#endif
	}

	WindowManager* WindowManager::GetInstance()
	{
		return mInstance;
	}
}