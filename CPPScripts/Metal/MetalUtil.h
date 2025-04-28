#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class MetalUtil
	{
	public:
		static void TranslateAllShaderToMSL(const string& path);

	private:
		static void TranslateShaderToMSL(const filesystem::path& path);
	};
}