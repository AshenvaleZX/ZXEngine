#include "EditorDataManager.h"

namespace ZXEngine
{
	EditorDataManager* EditorDataManager::mInstance = nullptr;

	void EditorDataManager::Create()
	{
		mInstance = new EditorDataManager();
	}

	EditorDataManager* EditorDataManager::GetInstance()
	{
		return mInstance;
	}

	EditorDataManager::EditorDataManager()
	{
		selectedGO = nullptr;
		selectedAsset = nullptr;
	}

	void EditorDataManager::SetSelectedGO(GameObject* go)
	{
		selectedGO = go;
		selectedAsset = nullptr;
	}

	void EditorDataManager::SetSelectedAsset(EditorAssetNode* asset)
	{
		selectedGO = nullptr;
		selectedAsset = asset;
	}
}