#include "../RenderEngine.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"
#include "EditorGUIManager.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorAssetPreviewer.h"
#include "EditorConsolePanel.h"

namespace ZXEngine
{
	EditorGUIManager* EditorGUIManager::mInstance = nullptr;

	void EditorGUIManager::Create()
	{
		mInstance = new EditorGUIManager();
	}

	EditorGUIManager* EditorGUIManager::GetInstance()
	{
		return mInstance;
	}

	EditorGUIManager::EditorGUIManager()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		const char* glsl_version = "#version 460";
		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(RenderEngine::GetInstance()->window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		allPanels.push_back(new EditorProjectPanel());
		allPanels.push_back(new EditorMainBarPanel());
		allPanels.push_back(new EditorInspectorPanel());
		allPanels.push_back(new EditorHierarchyPanel());
		allPanels.push_back(new EditorConsolePanel());
		assetPreviewer = new EditorAssetPreviewer();
	}

	EditorGUIManager::~EditorGUIManager()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorGUIManager::BeginEditorRender()
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManager::EditorRender()
	{
		if (assetPreviewer->Check())
			assetPreviewer->Draw();

		for (auto panel : allPanels)
		{
			panel->DrawPanel();
		}
		ImGui::Render();
	}

	void EditorGUIManager::EndEditorRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EditorGUIManager::ResetPanels()
	{
		for (auto panel : allPanels)
			panel->ResetPanel();
	}
}