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
		void OnWindowSizeChange();

	private:
		vector<EditorPanel*> allPanels;

#ifdef ZX_API_VULKAN
		void InitForVulkan();
		void FrameRender();
		void RecreateFrameBuffers();
#endif
	};
}