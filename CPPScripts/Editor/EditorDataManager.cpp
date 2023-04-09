#include "EditorDataManager.h"
#include "EditorGUIManager.h"
#include "EditorAssetPreviewer.h"
#include "../Texture.h"
#include "../Material.h"
#include "../MeshRenderer.h"

namespace ZXEngine
{
	bool EditorDataManager::isGameStart = false;
	bool EditorDataManager::isGamePause = false;
	EditorDataManager* EditorDataManager::mInstance = nullptr;

	void EditorDataManager::Create()
	{
		mInstance = new EditorDataManager();
	}

	EditorDataManager* EditorDataManager::GetInstance()
	{
		return mInstance;
	}

	void EditorDataManager::AddLog(LogType type, string msg)
	{
		auto newLog = new LogInfo();
		newLog->type = type;
		newLog->data = msg;

		if (type == LogType::Message)
			messageSize++;
		else if (type == LogType::Warning)
			warningSize++;
		else if (type == LogType::Error)
			errorSize++;

		if (logHead == nullptr)
		{
			logSize++;
			logHead = newLog;
			logTail = newLog;
		}
		else
		{
			logTail->next = newLog;
			logTail = newLog;

			if (logSize >= maxLogSize)
			{
				if (logHead->type == LogType::Message)
					messageSize--;
				else if (logHead->type == LogType::Warning)
					warningSize--;
				else if (logHead->type == LogType::Error)
					errorSize--;

				auto tmpLog = logHead;
				logHead = logHead->next;
				delete tmpLog;
			}
			else
			{
				logSize++;
			}
		}
	}

	void EditorDataManager::SetSelectedGO(GameObject* go)
	{
		selectedGO = go;
	}

	void EditorDataManager::SetSelectedAsset(EditorAssetNode* asset)
	{
		selectedGO = nullptr;
		if (curAssetInfo != nullptr)
		{
			// delete上一个AssetInfo的时候需要先把指针映射成对应类型，才能正确调用析构函数，确保内存正确释放
			if (selectedAsset->type == AssetType::Script)
				delete static_cast<AssetScriptInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Shader)
				delete static_cast<AssetShaderInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Texture)
				delete static_cast<AssetTextureInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Material)
				delete static_cast<AssetMaterialInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Model)
				delete static_cast<AssetModelInfo*>(curAssetInfo);
			else
				delete curAssetInfo;
			// delete后立刻重新赋值为nullptr，防止连续delete指针出现无法预期的行为导致直接崩溃
			curAssetInfo = nullptr;
		}
		selectedAsset = asset;

		if (asset->type == AssetType::Script)
		{
			auto info = new AssetScriptInfo();
			info->name = asset->name;
			info->preview = GetTextFilePreview(asset->path);
			curAssetInfo = info;
		}
		else if (asset->type == AssetType::Shader)
		{
			auto info = new AssetShaderInfo();
			info->name = asset->name;
			info->preview = GetTextFilePreview(asset->path);
			curAssetInfo = info;
		}
		else if (asset->type == AssetType::Texture)
		{
			auto info = new AssetTextureInfo();
			info->name = asset->name;
			info->format = asset->extension;
			info->texture = new Texture(asset->path.c_str());
			curAssetInfo = info;
		}
		else if (asset->type == AssetType::Material)
		{
			auto info = new AssetMaterialInfo();
			info->name = asset->name;
			string localPath = Resources::GetAssetLocalPath(asset->path);
			MaterialStruct* matStruct = Resources::LoadMaterial(localPath);
			info->material = new Material(matStruct);
			delete matStruct;
			curAssetInfo = info;
			EditorGUIManager::GetInstance()->assetPreviewer->Reset();
		}
		else if (asset->type == AssetType::Model)
		{
			auto info = new AssetModelInfo();
			info->name = asset->name;
			info->format = asset->extension;
			info->meshRenderer = new MeshRenderer();
			info->meshRenderer->LoadModel(asset->path);
			curAssetInfo = info;
			EditorGUIManager::GetInstance()->assetPreviewer->Reset(info->meshRenderer->boundsSizeX);
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