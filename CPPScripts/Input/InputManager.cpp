#include "InputManager.h"

#if defined(ZX_PLATFORM_DESKTOP)
#	if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN) || defined(ZX_API_METAL)
#		include "InputManagerGLFW.h"
#	elif defined(ZX_API_D3D12)
#		include "InputManagerWindows.h"
#	endif
#elif defined(ZX_PLATFORM_ANDROID)
#	include "InputManagerAndroid.h"
#elif defined(ZX_PLATFORM_IOS)
#	include "InputManagerIOS.h"
#endif

namespace ZXEngine
{
	InputManager* InputManager::mInstance = nullptr;

	void InputManager::Create()
	{
#if defined(ZX_PLATFORM_DESKTOP)
#	if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN) || defined(ZX_API_METAL)
		mInstance = new InputManagerGLFW();
#	elif defined(ZX_API_D3D12)
		mInstance = new InputManagerWindows();
#	endif
#elif defined(ZX_PLATFORM_ANDROID)
		mInstance = new InputManagerAndroid();
#elif defined(ZX_PLATFORM_IOS)
		mInstance = new InputManagerIOS();
#endif
	}

	InputManager* InputManager::GetInstance()
	{
		return mInstance;
	}
}