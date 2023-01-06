#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class Texture;
	class EditorConsolePanel : public EditorPanel
	{
	public:
		EditorConsolePanel();
		~EditorConsolePanel() {};

		virtual void DrawPanel();

	private:
		Texture* logIcons[3];
		const ImVec2 iconSize = ImVec2(20, 20);
		bool showMessage = true;
		bool showWarning = true;
		bool showError = true;
	};
}