#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorHierarchyPanel : public EditorPanel
	{
	public:
		EditorHierarchyPanel() {};
		~EditorHierarchyPanel() {};

		virtual void DrawPanel();
	};
}