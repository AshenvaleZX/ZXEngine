#include "ImGuiTextureManager.h"
#if defined(ZX_API_OPENGL)
#include "ImGuiTextureManagerOpenGL.h"
#elif defined(ZX_API_VULKAN)
#include "ImGuiTextureManagerVulkan.h"
#elif defined(ZX_API_D3D12)
#include "ImGuiTextureManagerD3D12.h"
#elif defined(ZX_API_METAL)
#include "ImGuiTextureManagerMetal.h"
#endif

namespace ZXEngine
{
	ImGuiTextureManager* ImGuiTextureManager::mInstance = nullptr;

	void ImGuiTextureManager::Creat()
	{
#if defined(ZX_API_OPENGL)
		mInstance = new ImGuiTextureManagerOpenGL();
#elif defined(ZX_API_VULKAN)
		mInstance = new ImGuiTextureManagerVulkan();
#elif defined(ZX_API_D3D12)
		mInstance = new ImGuiTextureManagerD3D12();
#elif defined(ZX_API_METAL)
		mInstance = new ImGuiTextureManagerMetal();
#endif
	}

	ImGuiTextureManager* ImGuiTextureManager::GetInstance()
	{
		return mInstance;
	}
}