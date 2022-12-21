#include "EditorDataManager.h"
#include "../Texture.h"

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
		if (curAssetInfo != nullptr)
		{
			// delete上一个AssetInfo的时候需要先把指针映射成对应类型，才能正确调用析构函数，确保内存正确释放
			if (selectedAsset->type == AssetType::AT_Script)
				delete static_cast<AssetScriptInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::AT_Shader)
				delete static_cast<AssetShaderInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::AT_Texture)
				delete static_cast<AssetTextureInfo*>(curAssetInfo);
			else
				delete curAssetInfo;
			// delete后立刻重新赋值为nullptr，防止连续delete指针出现无法预期的行为导致直接崩溃
			curAssetInfo = nullptr;
		}
		selectedAsset = asset;

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
		else if (asset->type == AssetType::AT_Texture)
		{
			auto info = new AssetTextureInfo();
			info->name = asset->name;
			info->format = asset->extension;
			info->texture = new Texture(asset->path.c_str());
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