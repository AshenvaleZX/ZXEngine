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
		// 是否为合法单词字符
		static bool IsValidWordChar(char c);
		// 查找字符串中是否存在某个单词，并返回起始位
		static size_t FindWord(const std::string& str, const std::string& word, size_t offset);
		// 替代字符串中的单词
		static void ReplaceAllWord(std::string& oriStr, const std::string& srcWord, const std::string& dstWord);
		// 替代字符串中的字符串
		static void ReplaceAllString(std::string& oriStr, const std::string& srcStr, const std::string& dstStr);
		// 获取由sChar和eChar括起来的一段字符串起止位置
		static void GetNextStringBlockPos(std::string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos);
		// 字符串拼接
		static std::string ConcatenateStrings(const std::vector<std::string>& strings);
		static std::string ConvertPathToWindowsFormat(std::string path);
	};
}