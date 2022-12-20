#include "EditorHierarchyPanel.h"
#include "EditorDataManager.h"
#include "../Scene.h"
#include "../SceneManager.h"
#include "../GameObject.h"

namespace ZXEngine
{
	void EditorHierarchyPanel::DrawPanel()
	{
		// ����С��λ��
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::hierarchyWidth, (float)ProjectSetting::hierarchyHeight));

		// ��������������
		if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			auto scene = SceneManager::GetInstance()->GetCurScene();
			auto goNum = scene->gameObjects.size();
			static unsigned int selected = -1;
			for (unsigned int i = 0; i < goNum; i++)
			{
				auto gameObject = scene->gameObjects[i];
				if (ImGui::Selectable(gameObject->name.c_str(), selected == i))
				{
					selected = i;
					EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
				}
			}
		}
		ImGui::End();
	}
}