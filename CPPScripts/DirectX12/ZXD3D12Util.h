#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class ZXD3D12Util
	{
	public:
		static void TranslateAllShaderToHLSL(const string& path);

	private:
		static void TranslateShaderToHLSL(const filesystem::path& path);
	};
}