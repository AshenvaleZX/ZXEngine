#include "EditorHierarchyPanel.h"
#include "../Scene.h"
#include "../SceneManager.h"
#include "../GameObject.h"

namespace ZXEngine
{
	void EditorHierarchyPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::hierarchyWidth, (float)ProjectSetting::hierarchyHeight));

		// 设置面板具体内容
		ImGui::Begin("Hierarchy");
		{
			auto scene = SceneManager::GetInstance()->GetCurScene();
			for (auto gameObject : scene->gameObjects)
			{
				ImGui::Text(gameObject->name.c_str());
			}
		}
		ImGui::End();
	}
}