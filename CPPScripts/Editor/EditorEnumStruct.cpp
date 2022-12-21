#include "EditorEnumStruct.h"
#include "../Texture.h"

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
}