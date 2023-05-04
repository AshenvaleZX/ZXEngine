#include "RenderAPI.h"
#ifdef ZX_API_OPENGL
#include "RenderAPIOpenGL.h"
#endif
#ifdef ZX_API_VULKAN
#include "RenderAPIVulkan.h"
#endif
#ifdef ZX_API_D3D12
#include "RenderAPID3D12.h"
#include "DirectX12/ZXD3D12DescriptorManager.h"
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
#ifdef ZX_API_D3D12
		mInstance = new RenderAPID3D12();
		ZXD3D12DescriptorManager::Creat();
#endif
	}

	RenderAPI* RenderAPI::GetInstance()
	{
		return mInstance;
	}
}