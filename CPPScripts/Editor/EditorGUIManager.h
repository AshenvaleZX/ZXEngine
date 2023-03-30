#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorAssetPreviewer;
	class EditorGUIManager
	{
	public:
		static void Create();
		static EditorGUIManager* GetInstance();

	private:
		static EditorGUIManager* mInstance;

	public:
		EditorAssetPreviewer* assetPreviewer;

		EditorGUIManager();
		~EditorGUIManager();

		void BeginEditorRender();
		void EditorRender();
		void EndEditorRender();
		void ResetPanels();

	private:
		vector<EditorPanel*> allPanels;

#ifdef ZX_API_VULKAN
		void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
		void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
		void FramePresent(ImGui_ImplVulkanH_Window* wd);
#endif
	};
}