#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class ZXD3D12Util
	{
	public:
		static void CompileAllShader(string path);
		static void TranslateAllShaderToHLSL(const string& path);

	private:
		static void CompileShader(const filesystem::path& path);
		static void TranslateShaderToHLSL(const filesystem::path& path);
		static string TranslateShaderToHLSL(const filesystem::path& path, ShaderStageFlags& stageFlags);
	};
}