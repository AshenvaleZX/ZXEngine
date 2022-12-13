#include "EditorProjectPanel.h"
#include "../Resources.h"

namespace ZXEngine
{
	void EditorProjectPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::projectWidth, (float)ProjectSetting::projectHeight));

		// 设置面板具体内容
		ImGui::Begin("Peoject");
		{
			string path = Resources::GetAssetsPath();
			for (const auto& entry : filesystem::directory_iterator(path))
				ImGui::Text(entry.path().filename().string().c_str());
		}
		ImGui::End();
	}
}