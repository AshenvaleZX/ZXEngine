#pragma once
#include "EditorGUIManager.h"

namespace ZXEngine
{
	class EditorGUIManagerVulkan : public EditorGUIManager
	{
	public:
		EditorGUIManagerVulkan();
		~EditorGUIManagerVulkan();

		virtual void OnWindowSizeChange();

	protected:
		virtual void BeginEditorRender();
		virtual void EditorRender();
		virtual void EndEditorRender();

	private:
		void InitForVulkan();
		void FrameRender();
		void RecreateFrameBuffers();
	};
}