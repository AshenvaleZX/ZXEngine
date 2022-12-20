#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	class Texture;
	struct EditorAssetNode;
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

		Texture* fileIcons[9];
		unordered_map<string, int> extIconMap;
		void InitIcons();
		Texture* GetIcon(string extension);
	};
}