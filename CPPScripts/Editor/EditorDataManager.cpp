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
}