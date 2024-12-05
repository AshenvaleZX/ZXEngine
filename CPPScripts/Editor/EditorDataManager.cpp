#include "EditorDataManager.h"
#include "EditorGUIManager.h"
#include "EditorAssetPreviewer.h"
#include "../Time.h"
#include "../Utils.h"
#include "../Texture.h"
#include "../Material.h"
#include "../Component/MeshRenderer.h"
#include "../SceneManager.h"
#include "../ModelUtil.h"
#include "../ZMesh.h"
#include "../Audio/ZAudio.h"

#define ZX_EDITOR_ASYNC_LOAD

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
		std::lock_guard lock(logMutex);

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
		DeleteCurAssetInfo();
		selectedAsset = nullptr;
	}

	void EditorDataManager::SetSelectedAsset(EditorAssetNode* asset)
	{
		long long curTime = Time::curSysTime_micro;
		long long dt = curTime - lastClickTime;
		lastClickTime = curTime;

		// 点击同一个Asset
		if (selectedAsset == asset)
		{
			// 如果间隔时间小于300ms，则认为是双击
			if (dt < 300000)
			{

				switch (asset->type)
				{
				case AssetType::Scene:
					SceneManager::GetInstance()->LoadScene(Resources::GetAssetLocalPath(asset->path));
					break;
				case AssetType::Texture:
				case AssetType::Model:
				case AssetType::Script:
				case AssetType::Audio:
				case AssetType::Text:
#ifdef _WIN64
					system(("start " + asset->path).c_str());
#elif __APPLE__
					system(("open " + asset->path).c_str());
#endif
					break;
				default:
					break;
				}
			}

			// 无论是否双击，都不需要继续执行后续代码(更新当前选中资源)
			return;
		}

		selectedGO = nullptr;
		DeleteCurAssetInfo();
		selectedAsset = asset;

		if (asset->type == AssetType::Script || asset->type == AssetType::Text)
		{
			auto info = new AssetScriptInfo();
			info->name = asset->name;
			info->preview = GetTextFilePreview(asset->path);
			curAssetInfo = info;
		}
		else if (asset->type == AssetType::Shader || asset->type == AssetType::RayTracingShader)
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
			info->texture = new Texture(asset->path);
			curAssetInfo = info;
		}
		else if (asset->type == AssetType::Material || asset->type == AssetType::RayTracingMaterial || asset->type == AssetType::DeferredMaterial)
		{
			auto info = new AssetMaterialInfo();
			info->name = asset->name;
			curAssetInfo = info;
			string localPath = Resources::GetAssetLocalPath(asset->path);

#ifdef ZX_EDITOR_ASYNC_LOAD
			Resources::AsyncLoadMaterial(localPath, [this](MaterialStruct* matStruct)
			{
				auto curInfo = static_cast<AssetMaterialInfo*>(this->curAssetInfo);
				curInfo->material = new Material(matStruct);
				EditorGUIManager::GetInstance()->assetPreviewer->Reset();
		}, false, true);
#else
			MaterialStruct* matStruct = Resources::LoadMaterial(localPath);
			info->material = new Material(matStruct);
			delete matStruct;
			EditorGUIManager::GetInstance()->assetPreviewer->Reset();
#endif
		}
		else if (asset->type == AssetType::Model)
		{
			auto info = new AssetModelInfo();
			info->name = asset->name;
			info->format = asset->extension;
			curAssetInfo = info;

#ifdef ZX_EDITOR_ASYNC_LOAD
			Resources::AsyncLoadModelData(asset->path, [this](ModelData* modelData)
			{
				auto curInfo = static_cast<AssetModelInfo*>(this->curAssetInfo);
				curInfo->meshRenderer = new MeshRenderer();
				
				for (auto& mesh : modelData->pMeshes)
				{
					mesh->SetUp();
				}
				curInfo->meshRenderer->SetMeshes(modelData->pMeshes);
				
				curInfo->boneNum = modelData->boneNum;
				curInfo->animBriefInfos = std::move(modelData->animBriefInfos);

				float size = std::max({ curInfo->meshRenderer->mAABBSizeX, curInfo->meshRenderer->mAABBSizeY, curInfo->meshRenderer->mAABBSizeZ });
				EditorGUIManager::GetInstance()->assetPreviewer->Reset(size);
			}, false, true);
#else
			ModelData* pModelData = ModelUtil::LoadModel(asset->path, false);
			info->meshRenderer = new MeshRenderer();
			info->meshRenderer->SetMeshes(pModelData->pMeshes);
			info->boneNum = pModelData->boneNum;
			info->animBriefInfos = std::move(pModelData->animBriefInfos);
			delete pModelData;

			float size = std::max({ info->meshRenderer->mAABBSizeX, info->meshRenderer->mAABBSizeY, info->meshRenderer->mAABBSizeZ });
			EditorGUIManager::GetInstance()->assetPreviewer->Reset(size);
#endif
		}
		else if (asset->type == AssetType::Audio)
		{
			auto info = new AssetAudioInfo();
			info->name = asset->name + asset->extension;
			info->sizeStr = Utils::DataSizeToString(asset->size);
			info->audioClip = AudioEngine::GetInstance()->CreateAudioClip(asset->path, true);
			info->lengthStr = Utils::MillisecondsToString(info->audioClip->GetLengthMS());
			curAssetInfo = info;
		}
	}

	void EditorDataManager::DeleteCurAssetInfo()
	{
		if (curAssetInfo != nullptr)
		{
			// delete上一个AssetInfo的时候需要先把指针映射成对应类型，才能正确调用析构函数，确保内存正确释放
			if (selectedAsset->type == AssetType::Script ||
				selectedAsset->type == AssetType::Text)
				delete static_cast<AssetScriptInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Shader ||
				selectedAsset->type == AssetType::RayTracingShader)
				delete static_cast<AssetShaderInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Texture)
				delete static_cast<AssetTextureInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Material ||
				selectedAsset->type == AssetType::RayTracingMaterial ||
				selectedAsset->type == AssetType::DeferredMaterial)
				delete static_cast<AssetMaterialInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Model)
				delete static_cast<AssetModelInfo*>(curAssetInfo);
			else if (selectedAsset->type == AssetType::Audio)
				delete static_cast<AssetAudioInfo*>(curAssetInfo);
			else
				delete curAssetInfo;
			// delete后立刻重新赋值为nullptr，防止连续delete指针出现无法预期的行为导致直接崩溃
			curAssetInfo = nullptr;
#ifdef ZX_EDITOR
			Resources::ClearEditorAsyncLoad();
#endif
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