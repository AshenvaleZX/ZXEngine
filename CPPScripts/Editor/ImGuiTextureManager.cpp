#include "ImGuiTextureManager.h"
#ifdef ZX_API_OPENGL
#include "ImGuiTextureManagerOpenGL.h"
#endif
#ifdef ZX_API_VULKAN
#include "ImGuiTextureManagerVulkan.h"
#endif
#ifdef ZX_API_D3D12
#include "ImGuiTextureManagerD3D12.h"
#endif

namespace ZXEngine
{
	ImGuiTextureManager* ImGuiTextureManager::mInstance = nullptr;

	void ImGuiTextureManager::Creat()
	{
#ifdef ZX_API_OPENGL
		mInstance = new ImGuiTextureManagerOpenGL();
#endif
#ifdef ZX_API_VULKAN
		mInstance = new ImGuiTextureManagerVulkan();
#endif
#ifdef ZX_API_D3D12
		mInstance = new ImGuiTextureManagerD3D12();
#endif
	}

	ImGuiTextureManager* ImGuiTextureManager::GetInstance()
	{
		return mInstance;
	}
}