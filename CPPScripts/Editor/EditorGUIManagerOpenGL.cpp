#include "EditorGUIManagerOpenGL.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorConsolePanel.h"
#include "EditorAssetPreviewer.h"
#include "ImGuiTextureManager.h"
#include "../Window/WindowManager.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

namespace ZXEngine
{
	EditorGUIManagerOpenGL::EditorGUIManagerOpenGL()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		const char* glsl_version = "#version 460";
		ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(WindowManager::GetInstance()->GetWindow()), true);
		ImGui_ImplOpenGL3_Init(glsl_version);
	}

	EditorGUIManagerOpenGL::~EditorGUIManagerOpenGL()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorGUIManagerOpenGL::Init()
	{
		// Inspector的绘制要放在Hierarchy和Project后面，因为这两个面板会决定Inspector的内容
		allPanels.push_back(new EditorProjectPanel());
		allPanels.push_back(new EditorMainBarPanel());
		allPanels.push_back(new EditorHierarchyPanel());
		allPanels.push_back(new EditorInspectorPanel());
		allPanels.push_back(new EditorConsolePanel());
		assetPreviewer = new EditorAssetPreviewer();
	}

	void EditorGUIManagerOpenGL::BeginEditorRender()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManagerOpenGL::EditorRender()
	{
		if (assetPreviewer->Check())
			assetPreviewer->Draw();

		for (auto panel : allPanels)
		{
			panel->DrawPanel();
		}
		ImGui::Render();
	}

	void EditorGUIManagerOpenGL::EndEditorRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void EditorGUIManagerOpenGL::ResetPanels()
	{
		for (auto panel : allPanels)
			panel->ResetPanel();
	}

	void EditorGUIManagerOpenGL::OnWindowSizeChange()
	{

	}
}