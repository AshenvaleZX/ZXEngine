#pragma once
#include "EditorGUIManager.h"

namespace ZXEngine
{
	class EditorGUIManagerVulkan : public EditorGUIManager
	{
	public:
		EditorGUIManagerVulkan();
		~EditorGUIManagerVulkan();

		virtual void Init();
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();
		virtual void ResetPanels();
		virtual void OnWindowSizeChange();

	private:
		vector<EditorPanel*> allPanels;

		void InitForVulkan();
		void FrameRender();
		void RecreateFrameBuffers();
	};
}