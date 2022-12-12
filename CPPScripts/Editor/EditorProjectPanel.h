#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorProjectPanel : public EditorPanel
	{
	public:
		EditorProjectPanel() {};
		~EditorProjectPanel() {};

		virtual void DrawPanel();
	};
}