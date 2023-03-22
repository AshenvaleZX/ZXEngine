#include "RenderAPI.h"
#ifdef ZX_API_OPENGL
#include "RenderAPIOpenGL.h"
#endif
#ifdef ZX_API_VULKAN
#include "RenderAPIVulkan.h"
#endif

namespace ZXEngine
{
	RenderAPI* RenderAPI::mInstance = nullptr;

	void RenderAPI::Creat()
	{
#ifdef ZX_API_OPENGL
		mInstance = new RenderAPIOpenGL();
#endif
#ifdef ZX_API_VULKAN
		mInstance = new RenderAPIVulkan();
#endif
	}

	RenderAPI* RenderAPI::GetInstance()
	{
		return mInstance;
	}
}