#pragma once
#include "EditorPanel.h"
#include "EditorDataManager.h"
#include "ImGuiTextureManager.h"

namespace ZXEngine
{
	class EditorProjectPanel : public EditorPanel
	{
	public:
		EditorProjectPanel();
		~EditorProjectPanel() {};

		virtual void DrawPanel();

	private:
		// ��ǰѡ�е�id
		size_t selected = -1;
		EditorAssetNode* root;
		EditorAssetNode* curNode;
		void SetCurNode(EditorAssetNode* node);
		void GetChildren(EditorAssetNode* node);

		unordered_map<string, AssetType> extTypeMap;
		void InitAssetType();
		AssetType GetAssetType(const string& extension);

		// �ļ�����С
		const ImVec2 nameSize = ImVec2(72.0f, 20.0f);
		// �ļ�icon��С
		const ImVec2 iconSize = ImVec2(64.0f, 64.0f);
		ImGuiTextureIndex fileIcons[static_cast<size_t>(AssetType::Count)];
		void InitIcons();
	};
}