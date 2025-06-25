#include "EditorGUIManager.h"
#include "EditorCamera.h"
#include "ImGuiTextureManager.h"
#include "EditorDialogBoxManager.h"
#include "EditorSceneWidgetsRenderer.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorConsolePanel.h"
#include "EditorGameViewPanel.h"
#include "EditorAssetPreviewPanel.h"
#include "EditorAssetPreviewer.h"

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

	void EditorGUIManager::Init()
	{
		mAllPanels.resize(static_cast<size_t>(EditorPanelType::Count));

		// Inspector的绘制要放在Hierarchy和Project后面，因为这两个面板会决定Inspector的内容
		mAllPanels[static_cast<size_t>(EditorPanelType::ProjectPanel)]      = new EditorProjectPanel();
		mAllPanels[static_cast<size_t>(EditorPanelType::MainBarPanel)]      = new EditorMainBarPanel();
		mAllPanels[static_cast<size_t>(EditorPanelType::HierarchyPanel)]    = new EditorHierarchyPanel();
		mAllPanels[static_cast<size_t>(EditorPanelType::InspectorPanel)]    = new EditorInspectorPanel();
		mAllPanels[static_cast<size_t>(EditorPanelType::ConsolePanel)]      = new EditorConsolePanel();
		mAllPanels[static_cast<size_t>(EditorPanelType::GameViewPanel)]     = new EditorGameViewPanel();
		mAllPanels[static_cast<size_t>(EditorPanelType::AssetPreviewPanel)] = new EditorAssetPreviewPanel();

		mAssetPreviewer = new EditorAssetPreviewer();
	}

	void EditorGUIManager::ResetPanels()
	{
		for (auto panel : mAllPanels)
		{
			panel->ResetPanel();
		}
	}

	EditorPanel* EditorGUIManager::GetPanel(EditorPanelType type)
	{
		if (type < EditorPanelType::Count)
		{
			return mAllPanels[static_cast<size_t>(type)];
		}
		return nullptr;
	}
}