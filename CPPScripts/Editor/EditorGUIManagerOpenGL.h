#pragma once
#include "EditorGUIManager.h"

namespace ZXEngine
{
	class EditorGUIManagerOpenGL : public EditorGUIManager
	{
	public:
		EditorGUIManagerOpenGL();
		~EditorGUIManagerOpenGL();

		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();
		virtual void ResetPanels();
		virtual void OnWindowSizeChange();

private:
		vector<EditorPanel*> allPanels;
	};
}