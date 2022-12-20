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
		EditorAssetNode* root;
		EditorAssetNode* curNode;
		void SetCurNode(EditorAssetNode* node);
		void GetChildren(EditorAssetNode* node);

		unordered_map<string, AssetType> extTypeMap;
		void InitAssetType();
		AssetType GetAssetType(string extension);

		Texture* fileIcons[9];
		void InitIcons();
	};
}