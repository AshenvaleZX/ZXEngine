#include "EditorGUIManager.h"
#include "EditorCamera.h"
#include "ImGuiTextureManager.h"
#include "EditorDialogBoxManager.h"
#include "EditorSceneWidgetsRenderer.h"

#if defined(ZX_API_OPENGL)
#include "EditorGUIManagerOpenGL.h"
#elif defined(ZX_API_VULKAN)
#include "EditorGUIManagerVulkan.h"
#elif defined(ZX_API_D3D12)
#include "EditorGUIManagerDirectX12.h"
#elif defined(ZX_API_METAL)
#include "EditorGUIManagerMetal.h"
#endif

namespace ZXEngine
{
	EditorGUIManager* EditorGUIManager::mInstance = nullptr;

	void EditorGUIManager::Create()
	{
#if defined(ZX_API_OPENGL)
		mInstance = new EditorGUIManagerOpenGL();
#elif defined(ZX_API_VULKAN)
		mInstance = new EditorGUIManagerVulkan();
#elif defined(ZX_API_D3D12)
		mInstance = new EditorGUIManagerDirectX12();
#elif defined(ZX_API_METAL)
		mInstance = new EditorGUIManagerMetal();
#endif
		ImGuiTextureManager::Creat();
		EditorDialogBoxManager::Create();
		EditorSceneWidgetsRenderer::Create();
		mInstance->Init();
	}

	EditorGUIManager* EditorGUIManager::GetInstance()
	{
		return mInstance;
	}

	void EditorGUIManager::Render()
	{
		EditorCamera::GetInstance()->Update();
		EditorSceneWidgetsRenderer::GetInstance()->Render();

		mInstance->BeginEditorRender();
		mInstance->EditorRender();
		mInstance->EndEditorRender();
	}
}