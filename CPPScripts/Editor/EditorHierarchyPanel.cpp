#include "EditorHierarchyPanel.h"
#include "EditorDataManager.h"
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
		if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			nodeIdx = 0;

			auto newSelectedGO = EditorDataManager::GetInstance()->selectedGO;
			// 如果选中的物体发生变化，需要自动展开并移动到选中的物体
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
			// 叶子节点
			nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			if (!gameObject->IsActive()) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(160, 160, 160, 255));
			ImGui::TreeNodeEx((void*)(intptr_t)nodeIdx, nodeFlags, gameObject->name.c_str());
			if (!gameObject->IsActive()) ImGui::PopStyleColor();

			// 滚动到选中的项
			if (autoExpand && selectedGO == gameObject)
			{
				ImVec2 itemPos = ImGui::GetCursorScreenPos();
				ImGui::SetScrollHereY();
			}

			if (ImGui::IsItemClicked())
			{
				EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
				// 如果是在面板上点击选取的，不会触发autoExpand
				selectedGO = gameObject;
			}
		}
		else
		{
			// 中间节点
			if (autoExpand && selectedGO->IsChildOf(gameObject))
			{
				ImGui::SetNextItemOpen(true);
			}

			if (!gameObject->IsActive()) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(160, 160, 160, 255));
			bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)nodeIdx, nodeFlags, gameObject->name.c_str());
			if (!gameObject->IsActive()) ImGui::PopStyleColor();

			// 滚动到选中的项
			if (autoExpand && selectedGO == gameObject)
			{
				ImVec2 itemPos = ImGui::GetCursorScreenPos();
				ImGui::SetScrollHereY();
			}

			if (ImGui::IsItemClicked())
			{
				EditorDataManager::GetInstance()->SetSelectedGO(gameObject);
				// 如果是在面板上点击选取的，不会触发autoExpand
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