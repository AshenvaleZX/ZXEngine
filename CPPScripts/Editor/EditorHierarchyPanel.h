#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class GameObject;
	class EditorHierarchyPanel : public EditorPanel
	{
	public:
		EditorHierarchyPanel() {};
		~EditorHierarchyPanel() {};

		virtual void DrawPanel();
		virtual void ResetPanel();

	private:
		int nodeIdx = 0;
		GameObject* selectedGO = nullptr;
		ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		void DrawNode(GameObject* gameObject);
	};
}