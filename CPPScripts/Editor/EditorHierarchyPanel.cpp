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

			auto newSelectedGO = EditorDataManager::GetInstance()->selectedGO;
			// ���ѡ�е����巢���仯����Ҫ�Զ�չ�����ƶ���ѡ�е�����
			autoExpand = newSelectedGO != nullptr && newSelectedGO != selectedGO;
			selectedGO = newSelectedGO;
			
			auto scene = SceneManager::GetInstance()->GetCurScene();
			for (auto gameObject : scene->gameObjects)
				DrawNode(gameObject);
		}
		ImGui::End();
	}

	void EditorHierarchyPanel::ResetPanel()
	{
		selectedGO = nullptr;
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

			if (!gameObject->IsActive()) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(160, 160, 160, 255));
			ImGui::TreeNodeEx((void*)(intptr_t)nodeIdx, nodeFlags, gameObject->name.c_str());
			if (!gameObject->IsActive()) ImGui::PopStyleColor();

			// ������ѡ�е���
			if (autoExpand && selectedGO == gameObject)
			{
				ImVec2 itemPos = ImGui::GetCursorScreenPos();
				ImGui::SetScrollHereY();
			}

			if (ImGui::IsItemClicked())
			{
				EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
				// �����������ϵ��ѡȡ�ģ����ᴥ��autoExpand
				selectedGO = gameObject;
			}
		}
		else
		{
			// �м�ڵ�
			if (autoExpand && selectedGO->IsChildOf(gameObject))
			{
				ImGui::SetNextItemOpen(true);
			}

			if (!gameObject->IsActive()) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(160, 160, 160, 255));
			bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)nodeIdx, nodeFlags, gameObject->name.c_str());
			if (!gameObject->IsActive()) ImGui::PopStyleColor();

			// ������ѡ�е���
			if (autoExpand && selectedGO == gameObject)
			{
				ImVec2 itemPos = ImGui::GetCursorScreenPos();
				ImGui::SetScrollHereY();
			}

			if (ImGui::IsItemClicked())
			{
				EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
				// �����������ϵ��ѡȡ�ģ����ᴥ��autoExpand
				selectedGO = gameObject;
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