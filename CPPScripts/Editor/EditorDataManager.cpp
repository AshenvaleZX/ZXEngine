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
		curAssetInfo = nullptr;
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

		if (curAssetInfo != nullptr)
		{
			delete curAssetInfo;
			// delete后立刻重新赋值为nullptr，防止连续delete指针出现无法预期的行为导致直接崩溃
			curAssetInfo = nullptr;
		}
	
		if (asset->type == AssetType::AT_Script)
		{
			auto info = new AssetScriptInfo();
			info->name = asset->name;
			info->preview = GetTextFilePreview(asset->path);
			curAssetInfo = info;
		}
		else if (asset->type == AssetType::AT_Shader)
		{
			auto info = new AssetShaderInfo();
			info->name = asset->name;
			info->preview = GetTextFilePreview(asset->path);
			curAssetInfo = info;
		}
	}

	string EditorDataManager::GetTextFilePreview(string path)
	{
		ifstream f(path);
		if (f.is_open())
		{
			string line;
			string content;
			int lineNum = 40;
			while (getline(f, line) && lineNum > 0)
			{
				content = content + line + "\n";
				lineNum--;
			}
			return content;
		}
		else
		{
			Debug::LogError("Get text file preview failed: " + path);
			return "";
		}
	}
}