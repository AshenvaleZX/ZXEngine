#include "EditorProjectPanel.h"
#include "../Resources.h"

namespace ZXEngine
{
	// 在Project面板中屏蔽的文件类型
	unordered_set<string> ignoreExtensions = 
	{ 
		// 生成的HLSL代码
		".hlsl",
		// 预编译的D3D12 Shader
		".fxc",
		// 生成的GLSL代码
		".vert", ".frag", ".geom",
		// 预编译的Vulkan Shader
		".spv",
	};

	EditorProjectPanel::EditorProjectPanel()
	{
		InitIcons();
		InitAssetType();
		root = new EditorAssetNode();
		root->parent = nullptr;
		root->path = Resources::GetAssetsPath();
		root->name = "Assets";
		root->extension = "";
		root->type = AssetType::Folder;
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
			// 当前路径节点，如果当前节点不是文件夹，就用当前节点的父节点
			auto curPathNode = curNode->type == AssetType::Folder ? curNode : curNode->parent;

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
			for (auto i = pathNodes.size(); i > 0; i--)
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
			size_t childNum = curPathNode->children.size();
			// 当前窗口的x最大值(右边界位置)
			float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
			// 记录当前绘制的一排文件名
			vector<string> fileNames;
			// 当前绘制的文件在当前这一行里是第几个
			int rowIdx = 0;
			// 记录当前选中的文件在当前这一行里是第几个
			int curRowIdx = -1;
			for (size_t i = 0; i < childNum; i++)
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
				auto icon = fileIcons[(int)node->type];
				bool click = ImGui::ImageButton(label.c_str(), icon.ImGuiID, iconSize);
				ImGui::PopStyleColor(1);
				if (click)
				{
					selected = i;
					SetCurNode(node);
					if (node->type == AssetType::Folder)
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
					for (size_t j = 0; j < fileNames.size(); j++)
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
#ifdef __APPLE__
			string filename = entry.path().filename().string();
			// 跳过MacOS的.DS_Store(烦人的文件，不知道苹果搞这玩意干嘛)，filesystem处理这个文件会异常
			if (filename.compare(".DS_Store") == 0)
				continue;
#endif

			string extension = entry.path().filename().extension().string();
			// 如果是忽略的文件类型，就跳过
			if (ignoreExtensions.find(extension) != ignoreExtensions.end())
				continue;

			EditorAssetNode* child = new EditorAssetNode();
			child->parent = node;
			child->path = entry.path().string();
			child->name = entry.path().stem().string();
			child->extension = extension;

			if (entry.is_directory())
			{
				child->size = 0;
				child->type = AssetType::Folder;
			}
			else
			{
				child->size = static_cast<uint32_t>(entry.file_size());
				child->type = GetAssetType(child->extension);
			}
			node->children.push_back(child);

			// 排序
			std::sort(node->children.begin(), node->children.end(), 
				[](EditorAssetNode* a, EditorAssetNode* b) 
				{ 
					// 文件夹排在前面
					if (a->type == AssetType::Folder && b->type != AssetType::Folder)
						return true;
					if (a->type != AssetType::Folder && b->type == AssetType::Folder)
						return false;
					// 按名字排序
					return a->name < b->name;
				}
			);

			if (child->type == AssetType::Folder)
				GetChildren(child);
		}
	}

	void EditorProjectPanel::InitAssetType()
	{
		extTypeMap.insert(make_pair<string, AssetType>("",          AssetType::Folder            ));
		extTypeMap.insert(make_pair<string, AssetType>(".zxmat",    AssetType::Material          ));
		extTypeMap.insert(make_pair<string, AssetType>(".zxdrmat",  AssetType::DeferredMaterial  ));
		extTypeMap.insert(make_pair<string, AssetType>(".zxrtmat",  AssetType::RayTracingMaterial));
		extTypeMap.insert(make_pair<string, AssetType>(".zxprefab", AssetType::Prefab            ));
		extTypeMap.insert(make_pair<string, AssetType>(".lua",      AssetType::Script            ));
		extTypeMap.insert(make_pair<string, AssetType>(".zxshader", AssetType::Shader            ));

		extTypeMap.insert(make_pair<string, AssetType>(".dxr",      AssetType::RayTracingShader  ));
		extTypeMap.insert(make_pair<string, AssetType>(".vkr",      AssetType::RayTracingShader  ));

		extTypeMap.insert(make_pair<string, AssetType>(".png",      AssetType::Texture           ));
		extTypeMap.insert(make_pair<string, AssetType>(".tga",      AssetType::Texture           ));
		extTypeMap.insert(make_pair<string, AssetType>(".jpg",      AssetType::Texture           ));

		extTypeMap.insert(make_pair<string, AssetType>(".zxscene",  AssetType::Scene             ));

		extTypeMap.insert(make_pair<string, AssetType>(".obj",      AssetType::Model             ));
		extTypeMap.insert(make_pair<string, AssetType>(".fbx",      AssetType::Model             ));

		extTypeMap.insert(make_pair<string, AssetType>(".wav",      AssetType::Audio             ));
		extTypeMap.insert(make_pair<string, AssetType>(".ogg",      AssetType::Audio             ));
	}

	AssetType EditorProjectPanel::GetAssetType(const string& extension)
	{
		auto iter = extTypeMap.find(Utils::StringToLower(extension));
		if (iter == extTypeMap.end())
			return AssetType::Other;
		else
			return iter->second;
	}

	void EditorProjectPanel::InitIcons()
	{
		auto ImTextureMgr = ImGuiTextureManager::GetInstance();
		fileIcons[(int)AssetType::Other]              = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/other.png",    true));
		fileIcons[(int)AssetType::Folder]             = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/folder.png",   true));
		fileIcons[(int)AssetType::Material]           = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/material.png", true));
		fileIcons[(int)AssetType::DeferredMaterial]   = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/material.png", true));
		fileIcons[(int)AssetType::RayTracingMaterial] = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/material.png", true));
		fileIcons[(int)AssetType::Prefab]             = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/prefab.png",   true));
		fileIcons[(int)AssetType::Script]             = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/lua.png",      true));
		fileIcons[(int)AssetType::Shader]             = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/shader.png",   true));
		fileIcons[(int)AssetType::Texture]            = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/texture.png",  true));
		fileIcons[(int)AssetType::Scene]              = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/scene.png",    true));
		fileIcons[(int)AssetType::Model]              = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/model.png",    true));
		fileIcons[(int)AssetType::RayTracingShader]   = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/raytrace.png", true));
		fileIcons[(int)AssetType::Audio]              = ImTextureMgr->LoadTexture(Resources::GetAssetFullPath("Textures/icons/audio.png",    true));
	}
}