#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorGameViewPanel : public EditorPanel
	{
	public:
		EditorGameViewPanel() = default;
		~EditorGameViewPanel() = default;

		virtual void DrawPanel();

	private:
		bool mFirstDraw = true;
	};
}