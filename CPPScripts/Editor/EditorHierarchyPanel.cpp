#include "EditorHierarchyPanel.h"

namespace ZXEngine
{
	void EditorHierarchyPanel::DrawPanel()
	{
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::hierarchyWidth, (float)ProjectSetting::hierarchyHeight));
		ImGui::Begin("Hierarchy");
		ImGui::Text("This is Hierarchy.");
		ImGui::End();
	}
}