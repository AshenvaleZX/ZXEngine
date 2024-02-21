#pragma once
#include <vector>
#include <string>
#include <algorithm>

namespace ZXEngine
{
	class Utils
	{
	public:
		// �ַ���ת��ΪСд
		static inline std::string StringToLower(const std::string& str);
		// �ַ���ת��Ϊ��д
		static inline std::string StringToUpper(const std::string& str);
		// �ַ����ָ�
		static std::vector<std::string> StringSplit(const std::string& oriStr, char p);
		// ��ȡ�ַ����е����е���
		static std::vector<std::string> ExtractWords(const std::string& str);
		// �Ƿ�Ϊ�Ϸ������ַ�
		static bool IsValidWordChar(char c);
		// �����ַ������Ƿ����ĳ�����ʣ���������ʼλ
		static size_t FindWord(const std::string& str, const std::string& word, size_t offset);
		// ����ַ����еĵ���
		static void ReplaceAllWord(std::string& oriStr, const std::string& srcWord, const std::string& dstWord);
		// ����ַ����е��ַ���
		static void ReplaceAllString(std::string& oriStr, const std::string& srcStr, const std::string& dstStr);
		// ��ȡ��sChar��eChar��������һ���ַ�����ֹλ��
		static void GetNextStringBlockPos(std::string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos);
		// �ַ���ƴ��
		static std::string ConcatenateStrings(const std::vector<std::string>& strings);
		// ·��ת��ΪWindows��ʽ
		static inline std::string ConvertPathToWindowsFormat(const std::string path);
		// ��ȡ�ļ���չ��
		static inline std::string GetFileExtension(const std::string& path);
		// ʱ��(��)ת��Ϊ�ַ���
		static std::string SecondsToString(float seconds);
		static std::string SecondsToString(uint32_t seconds);
		// ʱ��(����)ת��Ϊ�ַ���
		static std::string MillisecondsToString(uint32_t milliseconds);
		// ���ݴ�С(Byte)ת��Ϊ�ַ���
		static std::string DataSizeToString(uint64_t size);
	};

	std::string Utils::StringToLower(const std::string& str)
	{
		std::string res = str;
		std::transform(res.begin(), res.end(), res.begin(), ::tolower);
		return res;
	}

	std::string Utils::StringToUpper(const std::string& str)
	{
		std::string res = str;
		std::transform(res.begin(), res.end(), res.begin(), ::toupper);
		return res;
	}

	std::string Utils::ConvertPathToWindowsFormat(const std::string path)
	{
		std::string res = path;
		std::replace(res.begin(), res.end(), '/', '\\');
		return res;
	}

	std::string Utils::GetFileExtension(const std::string& path)
	{
		size_t pos = path.rfind('.');
		if (pos == std::string::npos)
			return "";
		return path.substr(pos + 1);
	}
}