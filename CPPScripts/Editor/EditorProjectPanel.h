#pragma once
#include "EditorPanel.h"
#include "EditorDataManager.h"

namespace ZXEngine
{
	class Texture;
	class EditorProjectPanel : public EditorPanel
	{
	public:
		EditorProjectPanel();
		~EditorProjectPanel() {};

		virtual void DrawPanel();

	private:
		// 当前选中的id
		size_t selected = -1;
		EditorAssetNode* root;
		EditorAssetNode* curNode;
		void SetCurNode(EditorAssetNode* node);
		void GetChildren(EditorAssetNode* node);

		unordered_map<string, AssetType> extTypeMap;
		void InitAssetType();
		AssetType GetAssetType(string extension);

		// 文件名大小
		const ImVec2 nameSize = ImVec2(72.0f, 20.0f);
		// 文件icon大小
		const ImVec2 iconSize = ImVec2(64.0f, 64.0f);
		Texture* fileIcons[9];
		void InitIcons();
	};
}