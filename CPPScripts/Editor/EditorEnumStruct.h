#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	enum AssetType
	{
		AT_Other,
		AT_Folder,
		AT_Material,
		AT_Prefab,
		AT_Script,
		AT_Shader,
		AT_Texture,
		AT_Scene,
		AT_Model,
	};

	struct EditorAssetNode
	{
		string path;
		string name;
		string extension;
		AssetType type;
		EditorAssetNode* parent;
		vector<EditorAssetNode*> children;
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
}