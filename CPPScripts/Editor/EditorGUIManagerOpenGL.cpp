#include "EditorGUIManagerOpenGL.h"
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

	void EditorGUIManagerOpenGL::BeginEditorRender()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorGUIManagerOpenGL::EditorRender()
	{
		if (mAssetPreviewer->Check())
			mAssetPreviewer->Draw();

		FBOManager::GetInstance()->SwitchFBO(ScreenBuffer);

		for (auto panel : mAllPanels)
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

	void EditorGUIManagerOpenGL::OnWindowSizeChange()
	{

	}
}