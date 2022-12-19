#pragma once
#include "EditorPanel.h"

namespace ZXEngine
{
	struct EditorFileNode
	{
		string path;
		string name;
		string extension;
		EditorFileNode* parent;
		vector<EditorFileNode*> children;
	};

	class EditorProjectPanel : public EditorPanel
	{
	public:
		EditorProjectPanel();
		~EditorProjectPanel() {};

		virtual void DrawPanel();

	private:
		EditorFileNode* root;
		EditorFileNode* curNode;
		void GetChildren(EditorFileNode* node);
	};
}