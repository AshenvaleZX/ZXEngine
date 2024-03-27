#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Texture;
	class MaterialData
	{
	public:
		MaterialType type;
		bool isDirty = false;
		bool initialized = false;
		map<string, float> floatDatas;
		map<string, uint32_t> uintDatas;
		map<string, Vector2> vec2Datas;
		map<string, Vector3> vec3Datas;
		map<string, Vector4> vec4Datas;
		map<string, Vector4> colorDatas;
		vector<pair<string, Texture*>> textures;

		// 光追材质的数据信息
		uint32_t rtMaterialDataSize;
		vector<ShaderProperty> rtMaterialProperties;

		MaterialData(MaterialType type);
		~MaterialData();

		void Use();
		uint32_t GetID();
		uint32_t GetRTID();

	private:
		uint32_t ID = 0;
		uint32_t rtID = 0;
	};
}