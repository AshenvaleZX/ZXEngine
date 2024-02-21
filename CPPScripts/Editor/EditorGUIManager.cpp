#include "EditorGUIManager.h"
#include "ImGuiTextureManager.h"
#include "EditorDialogBoxManager.h"
#ifdef ZX_API_OPENGL
#include "EditorGUIManagerOpenGL.h"
#endif
#ifdef ZX_API_VULKAN
#include "EditorGUIManagerVulkan.h"
#endif
#ifdef ZX_API_D3D12
#include "EditorGUIManagerDirectX12.h"
#endif

namespace ZXEngine
{
	EditorGUIManager* EditorGUIManager::mInstance = nullptr;

	void EditorGUIManager::Create()
	{
#ifdef ZX_API_OPENGL
		mInstance = new EditorGUIManagerOpenGL();
#endif
#ifdef ZX_API_VULKAN
		mInstance = new EditorGUIManagerVulkan();
#endif
#ifdef ZX_API_D3D12
		mInstance = new EditorGUIManagerDirectX12();
#endif
		ImGuiTextureManager::Creat();
		EditorDialogBoxManager::Create();
		mInstance->Init();
	}

	EditorGUIManager* EditorGUIManager::GetInstance()
	{
		return mInstance;
	}

	void EditorGUIManager::Render()
	{
		mInstance->BeginEditorRender();
		mInstance->EditorRender();
		mInstance->EndEditorRender();
	}
}