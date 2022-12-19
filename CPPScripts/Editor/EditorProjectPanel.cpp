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
		// ����С��λ��
		ImGui::SetNextWindowPos(ImVec2(0, (float)ProjectSetting::mainBarHeight + (float)ProjectSetting::hierarchyHeight));
		ImGui::SetNextWindowSize(ImVec2((float)ProjectSetting::projectWidth, (float)ProjectSetting::projectHeight));

		// ��������������
		ImGui::Begin("Peoject", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		{
			// ��¼һ�°�ťԭ����ɫ
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 btnColor = style.Colors[ImGuiCol_Button];
			// ��ǰѡ�е�id
			static unsigned int selected = -1;
			// �ļ���ʾ��С
			ImVec2 buttonSize(50.0f, 50.0f);
			// ��ǰ·���ڵ㣬�����ǰ�ڵ㲻���ļ��У����õ�ǰ�ڵ�ĸ��ڵ�
			auto curPathNode = curNode->extension == "" ? curNode : curNode->parent;

			// ����·����
			ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
			auto tmpPathNode = curPathNode;
			vector<EditorFileNode*> pathNodes;
			while (tmpPathNode != nullptr)
			{
				pathNodes.push_back(tmpPathNode);
				tmpPathNode = tmpPathNode->parent;
			}
			// ������ƣ���root����ǰλ��
			for (unsigned int i = pathNodes.size(); i > 0; i--)
			{
				ImGui::SameLine(); 
				if (ImGui::SmallButton(pathNodes[i - 1]->name.c_str()))
				{
					curNode = pathNodes[i - 1];
					// �л�·����ʱ��ˢ��ѡ��״̬
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

			// ��ǰ·���ļ�����
			unsigned int childNum = curPathNode->children.size();
			// ��ǰ���ڵ�x���ֵ(�ұ߽�λ��)
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			for (unsigned int i = 0; i < childNum; i++)
			{
				auto node = curPathNode->children[i];
				if (i == selected)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, btnColor);

				// �����Ƿ���������PopStyleColor������û��ֱ��д��if��
				bool click = ImGui::Button(node->name.c_str(), buttonSize);
				ImGui::PopStyleColor(1);
				if (click)
				{
					selected = i;
					curNode = node;
					if (node->extension == "")
					{
						// �л�·����ʱ��ˢ��ѡ��״̬
						selected = -1;
						break;
					}
				}

				// �����Ƿ���
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
			// ���ļ���չ(��׺��)��ȡ��
			child->name = child->name.substr(0, child->name.length() - child->extension.length());
			node->children.push_back(child);
			if (child->extension == "")
				GetChildren(child);
		}
	}
}