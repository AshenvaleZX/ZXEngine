#pragma once
#include <vector>
#include <string>

namespace ZXEngine
{
	class Utils
	{
	public:
		static std::vector<std::string> StringSplit(std::string oriStr, char p);
		static std::string ConvertPathToWindowsFormat(std::string path);
	};
}