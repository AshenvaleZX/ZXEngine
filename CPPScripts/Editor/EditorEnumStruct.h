#pragma once
#include "../pubh.h"
#include "../PublicStruct.h"

namespace ZXEngine
{
	enum class AssetType
	{
		Other,
		Folder,
		Material,
		DeferredMaterial,
		RayTracingMaterial,
		Prefab,
		Script,
		Shader,
		Texture,
		Scene,
		Model,
		RayTracingShader,
		Audio,
		Text,
		Count,
	};

	enum class EditorAreaType
	{
		None,
		Game,
		AssetPreview,
	};

	enum class EditorPanelType
	{
		ProjectPanel,
		MainBarPanel,
		HierarchyPanel,
		InspectorPanel,
		ConsolePanel,
		GameViewPanel,
		AssetPreviewPanel,
		Count,
	};

	struct EditorAssetNode
	{
		string path;
		string name;
		string extension;
		uint32_t size = 0;
		AssetType type = AssetType::Other;
		EditorAssetNode* parent = nullptr;
		vector<EditorAssetNode*> children;
	};

	struct LogInfo
	{
		LogType type;
		string data;
		LogInfo* next = nullptr;
	};

	struct AssetScriptInfo
	{
		string name;
		string preview;
	};

	struct AssetShaderInfo
	{
		string name;
		string preview;
	};

	class Texture;
	struct AssetTextureInfo
	{
		string name;
		string format;
		Texture* texture;

		AssetTextureInfo();
		~AssetTextureInfo();
	};

	class Material;
	struct AssetMaterialInfo
	{
		string name;
		Material* material;

		AssetMaterialInfo();
		~AssetMaterialInfo();
	};

	class MeshRenderer;
	struct AssetModelInfo
	{
		string name;
		string format;
		MeshRenderer* meshRenderer;
		uint32_t boneNum;
		vector<AnimBriefInfo> animBriefInfos;

		AssetModelInfo();
		~AssetModelInfo();
	};

	class AudioClip;
	struct AssetAudioInfo
	{
		string name;
		string sizeStr;
		string lengthStr;
		AudioClip* audioClip;

		AssetAudioInfo();
		~AssetAudioInfo();
	};
}