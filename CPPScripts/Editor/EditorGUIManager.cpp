#include "EditorGUIManager.h"
#include "../RenderEngine.h"

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
		ImGui::StyleColorsDark();

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
		ImGui::SetNextWindowSize(ImVec2(250, 50));
		ImGui::SetNextWindowPos(ImVec2(0, 60));
		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text.");
		ImGui::End();

		// Rendering
		ImGui::Render();
	}

	void EditorGUIManager::EndEditorRender()
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}