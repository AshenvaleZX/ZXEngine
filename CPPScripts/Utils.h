#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Utils
	{
	public:
		// �ַ���ת��ΪСд
		static inline string StringToLower(const string& str);
		// �ַ���ת��Ϊ��д
		static inline string StringToUpper(const string& str);
		// �ַ����ָ�
		static vector<string> StringSplit(const string& oriStr, char p);
		// ��ȡ�ַ����е����е���
		static vector<string> ExtractWords(const string& str);
		// �Ƿ�Ϊ�Ϸ������ַ�
		static bool IsValidWordChar(char c);
		// �����ַ������Ƿ����ĳ�����ʣ���������ʼλ
		static size_t FindWord(const string& str, const string& word, size_t offset);
		// ����ַ����еĵ���
		static void ReplaceAllWord(string& oriStr, const string& srcWord, const string& dstWord);
		// ����ַ����е��ַ���
		static void ReplaceAllString(string& oriStr, const string& srcStr, const string& dstStr);
		// ��ȡ��sChar��eChar��������һ���ַ�����ֹλ��
		static void GetNextStringBlockPos(string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos);
		// �ַ���ƴ��
		static string ConcatenateStrings(const vector<string>& strings);
		// ·��ת��ΪWindows��ʽ
		static inline string ConvertPathToWindowsFormat(const string path);
		// ���·��ת��Ϊ����·��
		static string RelativePathToAbsolutePath(const string& rPath);
		// ��ȡ�ļ���չ��
		static inline string GetFileExtension(const string& path);
		// ʱ��(��)ת��Ϊ�ַ���
		static string SecondsToString(float seconds);
		static string SecondsToString(uint32_t seconds);
		// ʱ��(����)ת��Ϊ�ַ���
		static string MillisecondsToString(uint32_t milliseconds);
		// ���ݴ�С(Byte)ת��Ϊ�ַ���
		static string DataSizeToString(uint64_t size);


		// ��ȡ��ǰ��ִ���ļ�(ZXEngine)·��
		static string GetCurrentExecutableFilePath();
		// ����ϵͳ��Ϣ��
		static void ShowSystemMessageBox(const string& title, const string& message);
	};

	string Utils::StringToLower(const string& str)
	{
		string res = str;
		transform(res.begin(), res.end(), res.begin(), ::tolower);
		return res;
	}

	string Utils::StringToUpper(const string& str)
	{
		string res = str;
		transform(res.begin(), res.end(), res.begin(), ::toupper);
		return res;
	}

	string Utils::ConvertPathToWindowsFormat(const string path)
	{
		string res = path;
		replace(res.begin(), res.end(), '/', '\\');
		return res;
	}

	string Utils::GetFileExtension(const string& path)
	{
		size_t pos = path.rfind('.');
		if (pos == string::npos)
			return "";
		return path.substr(pos + 1);
	}
}