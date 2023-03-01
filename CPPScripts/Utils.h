#pragma once
#include <vector>
#include <string>

namespace ZXEngine
{
	class Utils
	{
	public:
		static std::vector<std::string> StringSplit(const std::string& oriStr, char p);
		static std::vector<std::string> ExtractWords(const std::string& str);
		static void ReplaceAllString(std::string& oriStr, const std::string& srcStr, const std::string& dstStr);
		static std::string ConvertPathToWindowsFormat(std::string path);
	};
}