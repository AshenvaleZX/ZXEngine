#include "EditorMainBarPanel.h"

namespace ZXEngine
{
	void EditorMainBarPanel::DrawPanel()
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::mainBarWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::Begin("ZXEngine");
		ImGui::Text("This is main bar.");
		ImGui::End();
	}
}