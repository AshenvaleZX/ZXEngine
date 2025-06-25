#pragma once
#include "EditorGUIManager.h"

namespace ZXEngine
{
	class EditorGUIManagerOpenGL : public EditorGUIManager
	{
	public:
		EditorGUIManagerOpenGL();
		~EditorGUIManagerOpenGL();

		virtual void OnWindowSizeChange();

	protected:
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();
	};
}