#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class ShaderParser
	{
	public:
		static ShaderInfo GetShaderInfo(const string& path);
		static void ParseShaderCode(const string& path, string& vertCode, string& geomCode, string& fragCode);
		static string TranslateToVulkan(const string& originCode);

	private:
		static ShaderStateSet GetShaderStateSet(const string& code);
		static string GetCodeBlock(const string& code, const string& blockName);
	};
}