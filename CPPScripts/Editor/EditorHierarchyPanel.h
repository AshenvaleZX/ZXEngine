#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class GameObject;
	class EditorHierarchyPanel : public EditorPanel
	{
	public:
		EditorHierarchyPanel() = default;
		~EditorHierarchyPanel() = default;

		virtual void DrawPanel();
		virtual void ResetPanel();

	private:
		int nodeIdx = 0;
		bool autoExpand = false;
		GameObject* selectedGO = nullptr;
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		void DrawNode(GameObject* gameObject);
	};
}