#include "EditorProjectPanel.h"
#include "EditorDataManager.h"
#include "../Resources.h"
#include "../Texture.h"

namespace ZXEngine
{
	EditorProjectPanel::EditorProjectPanel()
	{
		root = new EditorAssetNode();
		root->parent = nullptr;
		root->path = Resources::GetAssetsPath();
		root->name = "Assets";
		root->extension = "";
		GetChildren(root);
		curNode = root;
		InitIcons();
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
			// �ļ�����С
			ImVec2 nameSize(72.0f, 20.0f);
			// �ļ�icon��С
			ImVec2 iconSize(64.0f, 64.0f);
			// ��ǰ·���ڵ㣬�����ǰ�ڵ㲻���ļ��У����õ�ǰ�ڵ�ĸ��ڵ�
			auto curPathNode = curNode->extension == "" ? curNode : curNode->parent;

			// ����·����
			ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
			auto tmpPathNode = curPathNode;
			vector<EditorAssetNode*> pathNodes;
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
					SetCurNode(pathNodes[i - 1]);
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
			// ��¼��ǰ���Ƶ�һ���ļ���
			vector<string> fileNames;
			for (unsigned int i = 0; i < childNum; i++)
			{
				auto node = curPathNode->children[i];
				if (i == selected)
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f));
				else
					ImGui::PushStyleColor(ImGuiCol_Button, btnColor);

				// �����Ƿ���������PopStyleColor������û��ֱ��д��if��
				string label = "##File" + to_string(i);
				auto icon = GetIcon(node->extension);
				auto texID = icon->GetID();
				bool click = ImGui::ImageButton(label.c_str(), (void*)(intptr_t)texID, iconSize);
				ImGui::PopStyleColor(1);
				if (click)
				{
					selected = i;
					SetCurNode(node);
					if (node->extension == "")
					{
						// �л�·����ʱ��ˢ��ѡ��״̬
						selected = -1;
						break;
					}
				}
				fileNames.push_back(node->name);

				// �����Ƿ���
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + iconSize.x;
				if (i + 1 < childNum && next_button_x2 < window_visible_x2)
				{
					ImGui::SameLine();
				}
				else
				{
					// ������һ���ļ�ǰ���Ȱ���һ�е��ļ������Ƴ���
					ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
					for (unsigned int j = 0; j < fileNames.size(); j++)
					{
						if (j > 0)
							ImGui::SameLine();
						ImGui::Button(fileNames[j].c_str(), nameSize);
					}
					ImGui::PopStyleColor(1);
					fileNames.clear();
				}
			}
		}
		ImGui::End();
	}

	void EditorProjectPanel::SetCurNode(EditorAssetNode* node)
	{
		curNode = node;
		EditorDataManager::GetInstance()->SetSelectedAsset(node);
	}

	void EditorProjectPanel::GetChildren(EditorAssetNode* node)
	{
		for (const auto& entry : filesystem::directory_iterator(node->path))
		{
			EditorAssetNode* child = new EditorAssetNode();
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

	void EditorProjectPanel::InitIcons()
	{
		fileIcons[0] = new Texture(Resources::GetAssetFullPath("Textures/icons/other.png").c_str());

		extIconMap.insert(make_pair<string, int>("", 1));
		fileIcons[1] = new Texture(Resources::GetAssetFullPath("Textures/icons/folder.png").c_str());

		extIconMap.insert(make_pair<string, int>(".zxmat", 2));
		fileIcons[2] = new Texture(Resources::GetAssetFullPath("Textures/icons/material.png").c_str());

		extIconMap.insert(make_pair<string, int>(".zxprefab", 3));
		fileIcons[3] = new Texture(Resources::GetAssetFullPath("Textures/icons/prefab.png").c_str());

		extIconMap.insert(make_pair<string, int>(".lua", 4));
		fileIcons[4] = new Texture(Resources::GetAssetFullPath("Textures/icons/script.png").c_str());

		extIconMap.insert(make_pair<string, int>(".zxshader", 5));
		fileIcons[5] = new Texture(Resources::GetAssetFullPath("Textures/icons/shader.png").c_str());

		extIconMap.insert(make_pair<string, int>(".png", 6));
		extIconMap.insert(make_pair<string, int>(".tga", 6));
		fileIcons[6] = new Texture(Resources::GetAssetFullPath("Textures/icons/texture.png").c_str());

		extIconMap.insert(make_pair<string, int>(".zxscene", 7));
		fileIcons[7] = new Texture(Resources::GetAssetFullPath("Textures/icons/scene.png").c_str());

		extIconMap.insert(make_pair<string, int>(".obj", 8));
		extIconMap.insert(make_pair<string, int>(".FBX", 8));
		fileIcons[8] = new Texture(Resources::GetAssetFullPath("Textures/icons/model.png").c_str());
	}

	Texture* EditorProjectPanel::GetIcon(string extension)
	{
		auto iter = extIconMap.find(extension);
		if (iter == extIconMap.end())
			return fileIcons[0];
		else
			return fileIcons[iter->second];
	}
}