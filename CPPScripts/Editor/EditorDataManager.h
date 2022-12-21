#pragma once
#include "EditorEnumStruct.h"

namespace ZXEngine
{
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
		void* curAssetInfo;

		EditorDataManager();
		~EditorDataManager() {};

		void SetSelectedGO(GameObject* go);
		void SetSelectedAsset(EditorAssetNode* asset);

	private:
		string GetTextFilePreview(string path);
	};
}