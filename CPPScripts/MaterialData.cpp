#include "MaterialData.h"
#include "RenderAPI.h"
#include "Texture.h"

namespace ZXEngine
{
	MaterialData::MaterialData()
	{
		ID = RenderAPI::GetInstance()->CreateMaterialData();
		rtID = RenderAPI::GetInstance()->CreateRayTracingMaterialData();
	}

	MaterialData::~MaterialData()
	{
		RenderAPI::GetInstance()->DeleteMaterialData(ID);
		RenderAPI::GetInstance()->DeleteRayTracingMaterialData(rtID);
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

	uint32_t MaterialData::GetRTID()
	{
		return rtID;
	}
}