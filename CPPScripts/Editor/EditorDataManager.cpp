#include "EditorDataManager.h"
#include "EditorGUIManager.h"
#include "EditorAssetPreviewer.h"
#include "EditorCamera.h"
#include "../Time.h"
#include "../Utils.h"
#include "../Texture.h"
#include "../Material.h"
#include "../Component/MeshRenderer.h"
#include "../SceneManager.h"
#include "../ModelUtil.h"
#include "../ZMesh.h"
#include "../Audio/ZAudio.h"
#include "../GameObject.h"

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

	EditorDataManager::~EditorDataManager()
	{
		if (mTransPosWidget)
			delete mTransPosWidget;
		if (mTransRotWidget)
			delete mTransRotWidget;
		if (mTransScaleWidget)
			delete mTransScaleWidget;
	}

	void EditorDataManager::InitWidgets()
	{
		PrefabStruct* prefab = Resources::LoadPrefab("Prefabs/TransWidgetPos.zxprefab", true);
		mTransPosWidget = new GameObject(prefab);
		delete prefab;
		RecordWidgetAxisInfo(mTransPosWidgetColliders, mTransPosWidgetOrientations, mTransPosWidget);

		prefab = Resources::LoadPrefab("Prefabs/TransWidgetRot.zxprefab", true);
		mTransRotWidget = new GameObject(prefab);
		delete prefab;
		RecordRotWidgetAxisInfo(mTransRotWidgetColliders, mTransRotWidgetTurnplates, mTransRotWidget);

		prefab = Resources::LoadPrefab("Prefabs/TransWidgetScale.zxprefab", true);
		mTransScaleWidget = new GameObject(prefab);
		delete prefab;
		RecordWidgetAxisInfo(mTransScaleWidgetColliders, mTransScaleWidgetOrientations, mTransScaleWidget);
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
		long long curTime = Time::curSysTime_micro;
		long long dt = curTime - mLastGOClick;
		mLastGOClick = curTime;

		if (go != nullptr)
		{
			DeleteCurAssetInfo();
			selectedAsset = nullptr;

			if (selectedGO == go && dt < mDoubleClickInterval)
			{
				EditorCamera::GetInstance()->MoveTo(go);
			}
		}

		selectedGO = go;
	}

	void EditorDataManager::SetSelectedAsset(EditorAssetNode* asset)
	{
		long long curTime = Time::curSysTime_micro;
		long long dt = curTime - mLastAssetClick;
		mLastAssetClick = curTime;

		// 点击同一个Asset
		if (selectedAsset == asset)
		{
			// 双击
			if (dt < mDoubleClickInterval)
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
					Utils::OpenFileWithDefaultApplication(asset->path);
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

	GameObject* EditorDataManager::GetTransWidget() const
	{
		if (mCurTransType == TransformType::Position)
			return mTransPosWidget;
		else if (mCurTransType == TransformType::Rotation)
			return mTransRotWidget;
		else
			return mTransScaleWidget;
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
				Debug::LogError("DeleteCurAssetInfo failed, unknown asset type: %s", selectedAsset->type);

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

	void EditorDataManager::RecordWidgetAxisInfo(WidgetColliderMap& colliders, WidgetOrientationMap& orientations, GameObject* widget)
	{
		auto collider = widget->GetComponent<BoxCollider>();

		if (collider)
		{
			if (widget->name == "XCollider")
				colliders[AxisType::X] = collider;
			else if (widget->name == "YCollider")
				colliders[AxisType::Y] = collider;
			else if (widget->name == "ZCollider")
				colliders[AxisType::Z] = collider;
		}
		else
		{
			if (widget->name == "XHead")
				orientations[AxisType::X].first = widget;
			else if (widget->name == "XStick")
				orientations[AxisType::X].second = widget;
			else if (widget->name == "YHead")
				orientations[AxisType::Y].first = widget;
			else if (widget->name == "YStick")
				orientations[AxisType::Y].second = widget;
			else if (widget->name == "ZHead")
				orientations[AxisType::Z].first = widget;
			else if (widget->name == "ZStick")
				orientations[AxisType::Z].second = widget;
		}

		for (auto& child : widget->children)
		{
			RecordWidgetAxisInfo(colliders, orientations, child);
		}
	}

	void EditorDataManager::RecordRotWidgetAxisInfo(RotWidgetColliderMap& colliders, RotWidgetTurnplateMap& turnplates, GameObject* widget)
	{
		auto collider = widget->GetComponent<Circle2DCollider>();

		if (collider)
		{
			if (widget->name == "XCollider")
				colliders[AxisType::X] = collider;
			else if (widget->name == "YCollider")
				colliders[AxisType::Y] = collider;
			else if (widget->name == "ZCollider")
				colliders[AxisType::Z] = collider;
		}
		else
		{
			if (widget->name == "XTurnplate")
				turnplates[AxisType::X] = widget;
			else if (widget->name == "YTurnplate")
				turnplates[AxisType::Y] = widget;
			else if (widget->name == "ZTurnplate")
				turnplates[AxisType::Z] = widget;
		}

		for (auto& child : widget->children)
		{
			RecordRotWidgetAxisInfo(colliders, turnplates, child);
		}
	}
}