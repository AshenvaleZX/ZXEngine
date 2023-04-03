#include "MaterialData.h"
#include "RenderAPI.h"
#include "Texture.h"

namespace ZXEngine
{
	MaterialData::MaterialData()
	{
		ID = RenderAPI::GetInstance()->CreateMaterialData();
	}

	MaterialData::~MaterialData()
	{
		RenderAPI::GetInstance()->DeleteMaterialData(ID);
		for (auto& iter : textures)
			delete iter.second;
	}

	void MaterialData::Use()
	{
		RenderAPI::GetInstance()->UseMaterialData(ID);
	}

	uint32_t MaterialData::GetID()
	{
		return ID;
	}
}