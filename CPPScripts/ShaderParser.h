#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	struct PropertyAlignInfo
	{
		uint32_t size = 0;
		uint32_t align = 0;
		uint32_t arrayOffset = 0;
	};

	struct CustomShaderFunctionInfo
	{
		string name;
		vector<string> textures;
	};

	class MaterialData;
	class ShaderParser
	{
	public:
		static ShaderInfo GetShaderInfo(const string& code, GraphicsAPI api);
		static ComputeShaderInfo GetComputeShaderInfo(const string& code);
		static bool IsBasePropertyType(ShaderPropertyType type);
		static ShaderPropertiesInfo GetProperties(const string& stageCode);
		static void ParseShaderCode(const string& code, string& vertCode, string& geomCode, string& fragCode);
		static string TranslateToOpenGL(const string& originCode);
		static string TranslateToVulkan(const string& originCode, const ShaderPropertiesInfo& info);
		static string TranslateToD3D12(const string& originCode, const ShaderInfo& shaderInfo);
		static string TranslateToMetal(const string& originCode, const ShaderInfo& shaderInfo);
		static void SetUpRTMaterialData(MaterialData* materialData, GraphicsAPI api);

	private:
		static ShaderStateSet GetShaderStateSet(const string& code);
		static void GetInstanceInfo(const string& code, ShaderInstanceInfo& info);
		static void GetBufferInfos(const string& code, vector<ShaderBufferInfo>& infos);
		static string GetCodeBlock(const string& code, const string& blockName);
		static void GetPropertyNameAndArrayLength(const string& propertyStr, string& name, uint32_t& arrayLength);
		static void GetCustomShaderFunctionInfo(const string& code, vector<CustomShaderFunctionInfo>& infos);
		static void InsertTextureParamsForMetal(string& code, const vector<CustomShaderFunctionInfo>& infos, const vector<ShaderProperty>& textureProperties);

		static string PreprocessMacroDefine(const string& code, std::initializer_list<const char*> macros);

		static void SetUpPropertiesStd140(ShaderInfo& info);
		static PropertyAlignInfo GetPropertyAlignInfoStd140(ShaderPropertyType type, uint32_t arrayLength);
		static void SetUpPropertiesHLSL(ShaderInfo& info);
		static PropertyAlignInfo GetPropertyAlignInfoHLSL(ShaderPropertyType type, uint32_t arrayLength);
		static void SetUpPropertiesMSL(ShaderInfo& info);
		static PropertyAlignInfo GetPropertyAlignInfoMSL(ShaderPropertyType type, uint32_t arrayLength);

		static void SetPropertyAlignInfo(ShaderProperty& property, uint32_t& offset, GraphicsAPI api);
	};
}