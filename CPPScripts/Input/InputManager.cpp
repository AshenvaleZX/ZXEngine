#include "InputManager.h"
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
#include "InputManagerGLFW.h"
#endif
#ifdef ZX_API_D3D12
#include "InputManagerWindows.h"
#endif

namespace ZXEngine
{
	InputManager* InputManager::mInstance = nullptr;

	void InputManager::Create()
	{
#if defined(ZX_API_OPENGL) || defined(ZX_API_VULKAN)
		mInstance = new InputManagerGLFW();
#endif
#ifdef ZX_API_D3D12
		mInstance = new InputManagerWindows();
#endif
	}

	InputManager* InputManager::GetInstance()
	{
		return mInstance;
	}
}