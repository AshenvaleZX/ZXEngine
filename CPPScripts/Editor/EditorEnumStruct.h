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
		RayTracingMaterial,
		Prefab,
		Script,
		Shader,
		Texture,
		Scene,
		Model,
		Count,
	};

	enum class EditorAreaType
	{
		None,
		Game,
		AssetPreview,
	};

	struct EditorAssetNode
	{
		string path;
		string name;
		string extension;
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
		vector<AnimBriefInfo> animBriefInfos;

		AssetModelInfo();
		~AssetModelInfo();
	};
}