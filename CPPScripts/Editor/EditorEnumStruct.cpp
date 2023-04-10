#include "EditorEnumStruct.h"
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