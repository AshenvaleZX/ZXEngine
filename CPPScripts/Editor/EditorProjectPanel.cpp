#include "EditorProjectPanel.h"

namespace ZXEngine
{
	void EditorProjectPanel::DrawPanel()
	{
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::projectWidth, (float)ProjectSetting::projectHeight));
		ImGui::Begin("Peoject");
		ImGui::Text("This is Peoject.");
		ImGui::End();
	}
}