#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	struct UniformAlignInfo
	{
		uint32_t size = 0;
		uint32_t align = 0;
		uint32_t arrayOffset = 0;
	};

	struct D3D12ConstAlignInfo
	{
		uint32_t size = 0;
		uint32_t align = 0;
		uint32_t arrayOffset = 0;
	};

	class MaterialData;
	class ShaderParser
	{
	public:
		static ShaderInfo GetShaderInfo(const string& code, GraphicsAPI api);
		static bool IsBasePropertyType(ShaderPropertyType type);
		static ShaderPropertiesInfo GetProperties(const string& stageCode);
		static void ParseShaderCode(const string& code, string& vertCode, string& geomCode, string& fragCode);
		static string TranslateToOpenGL(const string& originCode);
		static string TranslateToVulkan(const string& originCode, const ShaderPropertiesInfo& info);
		static string TranslateToD3D12(const string& originCode, const ShaderInfo& shaderInfo);
		static void SetUpRTMaterialData(MaterialData* materialData, GraphicsAPI api);

	private:
		static ShaderStateSet GetShaderStateSet(const string& code);
		static string GetCodeBlock(const string& code, const string& blockName);
		static void GetPropertyNameAndArrayLength(const string& propertyStr, string& name, uint32_t& arrayLength);

		static string PreprocessMacroDefine(const string& code, const string& macro);

		static void SetUpPropertiesStd140(ShaderInfo& info);
		static UniformAlignInfo GetPropertyAlignInfoStd140(ShaderPropertyType type, uint32_t arrayLength);
		static void SetUpPropertiesHLSL(ShaderInfo& info);
		static D3D12ConstAlignInfo GetPropertyAlignInfoHLSL(ShaderPropertyType type, uint32_t arrayLength);

		static void SetUpRTMaterialDataStd140(MaterialData* materialData);
		static void SetAlignInfoStd140(ShaderProperty& property, uint32_t& offset);
	};
}