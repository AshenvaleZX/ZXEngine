#include "RenderAPI.h"
#ifdef ZX_API_OPENGL
#include "RenderAPIOpenGL.h"
#endif
#ifdef ZX_API_VULKAN
#include "RenderAPIVulkan.h"
#endif
#ifdef ZX_API_D3D12
#include "RenderAPID3D12.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
#ifdef ZX_API_D3D12
		mInstance = new RenderAPID3D12();
		static_cast<RenderAPID3D12*>(mInstance)->InitAfterConstructor();
#endif
	}

	RenderAPI* RenderAPI::GetInstance()
	{
		return mInstance;
	}
}