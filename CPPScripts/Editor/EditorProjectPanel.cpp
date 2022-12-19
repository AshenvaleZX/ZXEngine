#include "EditorProjectPanel.h"
#include "../Resources.h"

namespace ZXEngine
{
	EditorProjectPanel::EditorProjectPanel()
	{
		root = new EditorFileNode();
		root->parent = nullptr;
		root->path = Resources::GetAssetsPath();
		root->name = "Assets";
		root->extension = "";
		GetChildren(root);
		curNode = root;
	}

	void EditorProjectPanel::DrawPanel()
	{
		// 面板大小和位置
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::projectWidth, (float)ProjectSetting::projectHeight));

		// 设置面板具体内容
		ImGui::Begin("Peoject", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		{
			// 记录一下按钮原本颜色
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 btnColor = style.Colors[ImGuiCol_Button];
			// 当前选中的id
			static unsigned int selected = -1;
			// 文件显示大小
			ImVec2 buttonSize(50.0f, 50.0f);
			// 当前路径节点，如果当前节点不是文件夹，就用当前节点的父节点
			auto curPathNode = curNode->extension == "" ? curNode : curNode->parent;

			// 绘制路径条
			ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
			auto tmpPathNode = curPathNode;
			vector<EditorFileNode*> pathNodes;
			while (tmpPathNode != nullptr)
			{
				pathNodes.push_back(tmpPathNode);
				tmpPathNode = tmpPathNode->parent;
			}
			// 倒叙绘制，从root到当前位置
			for (unsigned int i = pathNodes.size(); i > 0; i--)
			{
				ImGui::SameLine(); 
				if (ImGui::SmallButton(pathNodes[i - 1]->name.c_str()))
				{
					curNode = pathNodes[i - 1];
					// 切换路径的时候刷新选中状态
					selected = -1;
				}
				if (i > 1)
				{
					ImGui::SameLine();
					ImGui::Text(">");
				}
			}
			ImGui::PopStyleColor(1);
			ImGui::Separator();

			// 当前路径文件数量
			unsigned int childNum = curPathNode->children.size();
			// 当前窗口的x最大值(右边界位置)
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (unsigned int i = 0; i < childNum; i++)
			{
				auto node = curPathNode->children[i];
				if (i == selected)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, btnColor);

				// 无论是否点击都必须PopStyleColor，所以没有直接写在if中
				bool click = ImGui::Button(node->name.c_str(), buttonSize);
				ImGui::PopStyleColor(1);
				if (click)
				{
					selected = i;
					curNode = node;
					if (node->extension == "")
					{
						// 切换路径的时候刷新选中状态
						selected = -1;
						break;
					}
				}

				// 计算是否换行
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonSize.x;
				if (i + 1 < childNum && next_button_x2 < window_visible_x2)
					ImGui::SameLine();
			}
		}
		ImGui::End();
	}

	void EditorProjectPanel::GetChildren(EditorFileNode* node)
	{
		for (const auto& entry : filesystem::directory_iterator(node->path))
		{
			EditorFileNode* child = new EditorFileNode();
			child->parent = node;
			child->path = entry.path().string();
			child->name = entry.path().filename().string();
			child->extension = entry.path().filename().extension().string();
			// 把文件扩展(后缀名)截取掉
			child->name = child->name.substr(0, child->name.length() - child->extension.length());
			node->children.push_back(child);
			if (child->extension == "")
				GetChildren(child);
		}
	}
}