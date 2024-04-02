#include "MaterialData.h"
#include "RenderAPI.h"
#include "Texture.h"

namespace ZXEngine
{
	MaterialData::MaterialData(MaterialType type)
	{
		this->type = type;
		if (type == MaterialType::Forward || type == MaterialType::Deferred)
			ID = RenderAPI::GetInstance()->CreateMaterialData();
		else if (type == MaterialType::RayTracing)
			rtID = RenderAPI::GetInstance()->CreateRayTracingMaterialData();
	}

	MaterialData::~MaterialData()
	{
		if (type == MaterialType::Forward || type == MaterialType::Deferred)
			RenderAPI::GetInstance()->DeleteMaterialData(ID);
		else if (type == MaterialType::RayTracing)
			RenderAPI::GetInstance()->DeleteRayTracingMaterialData(rtID);
		for (auto& iter : textures)
			delete iter.second;
	}

	void MaterialData::Use() const
	{
		RenderAPI::GetInstance()->UseMaterialData(ID);
	}

	uint32_t MaterialData::GetID() const
	{
		return ID;
	}

	uint32_t MaterialData::GetRTID() const
	{
		return rtID;
	}

	Texture* MaterialData::GetTexture(const string& name) const
	{
		for (auto& iter : textures)
		{
			if (iter.first == name)
				return iter.second;
		}
		return nullptr;
	}
}