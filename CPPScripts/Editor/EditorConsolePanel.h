#pragma once
#include "EditorPanel.h"
#include "ImGuiTextureManager.h"

namespace ZXEngine
{
	class EditorConsolePanel : public EditorPanel
	{
	public:
		EditorConsolePanel();
		~EditorConsolePanel() {};

		virtual void DrawPanel();

	private:
		ImGuiTextureIndex logIcons[3];
		const ImVec2 iconSize = ImVec2(20, 20);
		bool showMessage = true;
		bool showWarning = true;
		bool showError = true;
	};
}