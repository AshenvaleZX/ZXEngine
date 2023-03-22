#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class ShaderParser
	{
	public:
		static ShaderInfo GetShaderInfo(const string& code);
		static bool IsBasePropertyType(ShaderPropertyType type);
		static uint32_t GetPropertySize(ShaderPropertyType type);
		static ShaderPropertiesInfo GetProperties(const string& stageCode);
		static void ParseShaderCode(const string& code, string& vertCode, string& geomCode, string& fragCode);
		static string TranslateToOpenGL(const string& originCode);
		static string TranslateToVulkan(const string& originCode, const ShaderPropertiesInfo& info);

	private:
		static ShaderStateSet GetShaderStateSet(const string& code);
		static string GetCodeBlock(const string& code, const string& blockName);
		static void GetPropertyNameAndArrayLength(const string& propertyStr, string& name, uint32_t& arrayLength);
		static void SetUpProperties(ShaderInfo& info);
	};
}