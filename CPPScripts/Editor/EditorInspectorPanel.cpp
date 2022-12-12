#include "EditorInspectorPanel.h"

namespace ZXEngine
{
	void EditorInspectorPanel::DrawPanel()
	{
		ImGui::SetNextWindowPos(ImVec2((float)ProjectSetting::hierarchyWidth + (float)GlobalData::srcWidth, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::inspectorWidth, (float)ProjectSetting::inspectorHeight));
		ImGui::Begin("Inspector");
		ImGui::Text("This is Inspector.");
		ImGui::End();
	}
}