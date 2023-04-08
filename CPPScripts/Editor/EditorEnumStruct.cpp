#include "EditorEnumStruct.h"
#include "ImGuiTextureManager.h"
#include "../Texture.h"
#include "../Material.h"
#include "../MeshRenderer.h"

namespace ZXEngine
{
	AssetTextureInfo::AssetTextureInfo()
	{
		texture = NULL;
	}

	AssetTextureInfo::~AssetTextureInfo()
	{
		ImGuiTextureManager::GetInstance()->DeleteByEngineID(texture->GetID());
		delete texture;
	}

	AssetMaterialInfo::AssetMaterialInfo()
	{
		material = NULL;
	}

	AssetMaterialInfo::~AssetMaterialInfo()
	{
		delete material;
	}

	AssetModelInfo::AssetModelInfo()
	{
		meshRenderer = NULL;
	}

	AssetModelInfo::~AssetModelInfo()
	{
		delete meshRenderer;
	}
}