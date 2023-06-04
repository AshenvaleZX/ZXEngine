#include "MaterialData.h"
#include "RenderAPI.h"
#include "Texture.h"

namespace ZXEngine
{
	MaterialData::MaterialData(MaterialType type)
	{
		this->type = type;
		if (type == MaterialType::Rasterization)
			ID = RenderAPI::GetInstance()->CreateMaterialData();
		else if (type == MaterialType::RayTracing)
			rtID = RenderAPI::GetInstance()->CreateRayTracingMaterialData();
	}

	MaterialData::~MaterialData()
	{
		if (type == MaterialType::Rasterization)
			RenderAPI::GetInstance()->DeleteMaterialData(ID);
		else if (type == MaterialType::RayTracing)
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