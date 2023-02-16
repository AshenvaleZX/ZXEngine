#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	enum class AssetType
	{
		Other,
		Folder,
		Material,
		Prefab,
		Script,
		Shader,
		Texture,
		Scene,
		Model,
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
		LogInfo* next;
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

		AssetModelInfo();
		~AssetModelInfo();
	};
}