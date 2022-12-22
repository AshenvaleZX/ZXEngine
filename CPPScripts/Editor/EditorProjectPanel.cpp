#include "EditorProjectPanel.h"
#include "../Resources.h"
#include "../Texture.h"

namespace ZXEngine
{
	EditorProjectPanel::EditorProjectPanel()
	{
		InitIcons();
		InitAssetType();
		root = new EditorAssetNode();
		root->parent = nullptr;
		root->path = Resources::GetAssetsPath();
		root->name = "Assets";
		root->extension = "";
		root->type = AssetType::AT_Folder;
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
			ImVec4 selectBtnColor = ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f);
			ImVec4 textColor = style.Colors[ImGuiCol_Text];
			ImVec4 selectTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			// 当前选中的id
			static unsigned int selected = -1;
			// 文件名大小
			ImVec2 nameSize(72.0f, 20.0f);
			// 文件icon大小
			ImVec2 iconSize(64.0f, 64.0f);
			// 当前路径节点，如果当前节点不是文件夹，就用当前节点的父节点
			auto curPathNode = curNode->extension == "" ? curNode : curNode->parent;

			// 绘制路径条
			ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
			auto tmpPathNode = curPathNode;
			vector<EditorAssetNode*> pathNodes;
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
					SetCurNode(pathNodes[i - 1]);
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
			// 记录当前绘制的一排文件名
			vector<string> fileNames;
			// 当前绘制的文件在当前这一行里是第几个
			int rowIdx = 0;
			// 记录当前选中的文件在当前这一行里是第几个
			int curRowIdx = -1;
			for (unsigned int i = 0; i < childNum; i++)
			{
				auto node = curPathNode->children[i];
				if (i == selected)
				{
					curRowIdx = rowIdx;
					ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
				}

				// 无论是否点击都必须PopStyleColor，所以没有直接写在if中
				string label = "##File" + to_string(i);
				auto icon = fileIcons[node->type];
				auto texID = icon->GetID();
				bool click = ImGui::ImageButton(label.c_str(), (void*)(intptr_t)texID, iconSize);
				ImGui::PopStyleColor(1);
				if (click)
				{
					selected = i;
					SetCurNode(node);
					if (node->extension == "")
					{
						// 切换路径的时候刷新选中状态
						selected = -1;
						break;
					}
				}
				fileNames.push_back(node->name);

				// 计算是否换行
				float last_button_x2 = ImGui::GetItemRectMax().x;
				float next_button_x2 = last_button_x2 + style.ItemSpacing.x + iconSize.x;
				if (i + 1 < childNum && next_button_x2 < window_visible_x2)
				{
					rowIdx++;
					ImGui::SameLine();
				}
				else
				{
					rowIdx = 0;
					// 绘制下一行文件前，先把这一行的文件名绘制出来
					for (unsigned int j = 0; j < fileNames.size(); j++)
					{
						if (j > 0)
							ImGui::SameLine();

						if (curRowIdx == j)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, selectTextColor);
							ImGui::PushStyleColor(ImGuiCol_Button, selectBtnColor);
						}
						else
						{
							ImGui::PushStyleColor(ImGuiCol_Text, textColor);
							ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_WindowBg]);
						}

						ImGui::Button(fileNames[j].c_str(), nameSize);
						ImGui::PopStyleColor(2);
					}
					fileNames.clear();
					curRowIdx = -1;
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
			child->type = GetAssetType(child->extension);
			// 把文件扩展(后缀名)截取掉
			child->name = child->name.substr(0, child->name.length() - child->extension.length());
			node->children.push_back(child);
			if (child->extension == "")
				GetChildren(child);
		}
	}

	void EditorProjectPanel::InitAssetType()
	{
		extTypeMap.insert(make_pair<string, AssetType>("", AssetType::AT_Folder));
		extTypeMap.insert(make_pair<string, AssetType>(".zxmat", AssetType::AT_Material));
		extTypeMap.insert(make_pair<string, AssetType>(".zxprefab", AssetType::AT_Prefab));
		extTypeMap.insert(make_pair<string, AssetType>(".lua", AssetType::AT_Script));
		extTypeMap.insert(make_pair<string, AssetType>(".zxshader", AssetType::AT_Shader));

		extTypeMap.insert(make_pair<string, AssetType>(".png", AssetType::AT_Texture));
		extTypeMap.insert(make_pair<string, AssetType>(".tga", AssetType::AT_Texture));

		extTypeMap.insert(make_pair<string, AssetType>(".zxscene", AssetType::AT_Scene));

		extTypeMap.insert(make_pair<string, AssetType>(".obj", AssetType::AT_Model));
		extTypeMap.insert(make_pair<string, AssetType>(".FBX", AssetType::AT_Model));
	}

	AssetType EditorProjectPanel::GetAssetType(string extension)
	{
		auto iter = extTypeMap.find(extension);
		if (iter == extTypeMap.end())
			return AssetType::AT_Other;
		else
			return iter->second;
	}

	void EditorProjectPanel::InitIcons()
	{
		fileIcons[AT_Other]	   = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/other.png").c_str());
		fileIcons[AT_Folder]   = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/folder.png").c_str());
		fileIcons[AT_Material] = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/material.png").c_str());
		fileIcons[AT_Prefab]   = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/prefab.png").c_str());
		fileIcons[AT_Script]   = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/script.png").c_str());
		fileIcons[AT_Shader]   = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/shader.png").c_str());
		fileIcons[AT_Texture]  = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/texture.png").c_str());
		fileIcons[AT_Scene]    = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/scene.png").c_str());
		fileIcons[AT_Model]    = new Texture(Resources::GetBuiltInAssetPath("Textures/icons/model.png").c_str());
	}
}