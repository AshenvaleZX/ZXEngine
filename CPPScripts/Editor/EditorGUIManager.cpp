#include "EditorGUIManager.h"
#include "../RenderEngine.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"

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
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::hierarchyWidth, (float)ProjectSetting::hierarchyHeight));
		ImGui::Begin("Hierarchy");
		ImGui::Text("This is Hierarchy.");
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::fileWidth, (float)ProjectSetting::fileHeight));
		ImGui::Begin("Peoject");
		ImGui::Text("This is Peoject.");
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::hierarchyWidth + (float)GlobalData::srcWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::inspectorWidth, (float)ProjectSetting::inspectorHeight));
		ImGui::Begin("Inspector");
		ImGui::Text("This is Hierarchy.");
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::mainBarWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::Begin("ZXEngine");
		ImGui::Text("This is main bar.");
		ImGui::End();

		// Rendering
		ImGui::Render();
	}

	void EditorGUIManager::EndEditorRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}