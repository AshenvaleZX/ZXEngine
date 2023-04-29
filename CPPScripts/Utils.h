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
		// �Ƿ�Ϊ�Ϸ������ַ�
		static bool IsValidWordChar(char c);
		// �����ַ������Ƿ����ĳ�����ʣ���������ʼλ
		static size_t FindWord(const std::string& str, const std::string& word, size_t offset);
		// ����ַ����еĵ���
		static void ReplaceAllWord(std::string& oriStr, const std::string& srcWord, const std::string& dstWord);
		// ����ַ����е��ַ���
		static void ReplaceAllString(std::string& oriStr, const std::string& srcStr, const std::string& dstStr);
		static std::string ConvertPathToWindowsFormat(std::string path);
	};
}