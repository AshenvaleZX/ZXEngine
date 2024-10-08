#include "EditorGUIManagerOpenGL.h"
#include "EditorProjectPanel.h"
#include "EditorMainBarPanel.h"
#include "EditorHierarchyPanel.h"
#include "EditorInspectorPanel.h"
#include "EditorConsolePanel.h"
#include "EditorGameViewPanel.h"
#include "EditorAssetPreviewPanel.h"
#include "EditorAssetPreviewer.h"
#include "EditorDialogBoxManager.h"
#include "../FBOManager.h"
#include "../ProjectSetting.h"
#include "../Window/WindowManager.h"
#include "../External/ImGui/imgui_impl_glfw.h"
#include "../External/ImGui/imgui_impl_opengl3.h"

namespace ZXEngine
{
	EditorGUIManagerOpenGL::EditorGUIManagerOpenGL()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsLight();

		string version_str = "#version " + ProjectSetting::OpenGLVersion;
		const char* glsl_version = version_str.c_str();
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
		allPanels.push_back(new EditorGameViewPanel());
		allPanels.push_back(new EditorAssetPreviewPanel());
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

		FBOManager::GetInstance()->SwitchFBO(ScreenBuffer);

		for (auto panel : allPanels)
		{
			panel->DrawPanel();
		}

		EditorDialogBoxManager::GetInstance()->Draw();

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