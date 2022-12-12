#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorInspectorPanel : public EditorPanel
	{
	public:
		EditorInspectorPanel() {};
		~EditorInspectorPanel() {};

		virtual void DrawPanel();
	};
}