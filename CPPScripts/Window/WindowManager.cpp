#include "WindowManager.h"
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
#include "WindowManagerGLFW.h"
#endif
#ifdef ZX_API_D3D12
#include "WindowManagerWindows.h"
#endif

namespace ZXEngine
{
	WindowManager* WindowManager::mInstance = nullptr;

	void WindowManager::Creat()
	{
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
		mInstance = new WindowManagerGLFW();
#endif
#ifdef ZX_API_D3D12
		mInstance = new WindowManagerWindows();
#endif
	}

	WindowManager* WindowManager::GetInstance()
	{
		return mInstance;
	}
}