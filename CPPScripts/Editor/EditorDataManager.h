#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	enum AssetType
	{
		AT_Other,
		AT_Folder,
		AT_Material,
		AT_Prefab,
		AT_Script,
		AT_Shader,
		AT_Texture,
		AT_Scene,
		AT_Model,
	};

	struct EditorAssetNode
	{
		string path;
		string name;
		string extension;
		AssetType type;
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