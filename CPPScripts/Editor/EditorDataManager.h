#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	struct EditorAssetNode
	{
		string path;
		string name;
		string extension;
		EditorAssetNode* parent;
		vector<EditorAssetNode*> children;
	};

	class GameObject;
	class EditorDataManager
	{
	public:
		static void Create();
		static EditorDataManager* GetInstance();

	private:
		static EditorDataManager* mInstance;

	public:
		GameObject* selectedGO;
		EditorAssetNode* selectedAsset;

		EditorDataManager();
		~EditorDataManager() {};

		void SetSelectedGO(GameObject* go);
		void SetSelectedAsset(EditorAssetNode* asset);
	};
}