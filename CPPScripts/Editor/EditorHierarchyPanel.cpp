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
			nodeIdx = 0;
			auto scene = SceneManager::GetInstance()->GetCurScene();
			for (auto gameObject : scene->gameObjects)
				DrawNode(gameObject);
		}
		ImGui::End();
	}

	void EditorHierarchyPanel::DrawNode(GameObject* gameObject)
	{
		nodeIdx++;

		ImGuiTreeNodeFlags nodeFlags = baseFlags;
		if (selectedGO == gameObject)
			nodeFlags |= ImGuiTreeNodeFlags_Selected;

		if (gameObject->children.size() == 0)
		{
			// Ҷ�ӽڵ�
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			ImGui::TreeNodeEx((void*)(intptr_t)nodeIdx, nodeFlags, gameObject->name.c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				selectedGO = gameObject;
				EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
			}
		}
		else
		{
			// �м�ڵ�
			bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)nodeIdx, nodeFlags, gameObject->name.c_str());
			if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			{
				selectedGO = gameObject;
				EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
			}

			if (nodeOpen)
			{
				for (auto subGameObject : gameObject->children)
					DrawNode(subGameObject);
				ImGui::TreePop();
			}
		}
	}
}