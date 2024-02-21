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
		virtual void ResetPanels();
		virtual void OnWindowSizeChange();

	protected:
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();

	private:
		vector<EditorPanel*> allPanels;

		void InitForVulkan();
		void FrameRender();
		void RecreateFrameBuffers();
	};
}