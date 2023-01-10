#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class EditorMainBarPanel : public EditorPanel
	{
	public:
		EditorMainBarPanel();
		~EditorMainBarPanel() {};

		virtual void DrawPanel();

	private:
		ImVec4 selectBtnColor;
		ImVec4 selectTextColor;
		const ImVec2 buttonSize = ImVec2(22.0f, 22.0f);
	};
}