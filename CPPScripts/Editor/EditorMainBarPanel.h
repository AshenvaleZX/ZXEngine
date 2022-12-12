#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorMainBarPanel : public EditorPanel
	{
	public:
		EditorMainBarPanel() {};
		~EditorMainBarPanel() {};

		virtual void DrawPanel();
	};
}