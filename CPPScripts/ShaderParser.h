#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class ShaderParser
	{
	public:
		static void ParseFile(string path, ShaderData& data);
		static string TranslateToVulkan(const string& originCode);

	private:
		static ShaderStateSet GetShaderStateSet(const string& code);
	};
}