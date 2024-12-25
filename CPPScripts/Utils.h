#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Utils
	{
	public:
		// 字符串转换为小写
		static inline string StringToLower(const string& str);
		// 字符串转换为大写
		static inline string StringToUpper(const string& str);
		// 字符串分割
		static vector<string> StringSplit(const string& oriStr, char p);
		// 提取字符串中的所有单词
		static vector<string> ExtractWords(const string& str);
		// 是否为合法单词字符
		static bool IsValidWordChar(char c);
		// 查找字符串中是否存在某个单词，并返回起始位
		static size_t FindWord(const string& str, const string& word, size_t offset);
		// 替代字符串中的单词
		static void ReplaceAllWord(string& oriStr, const string& srcWord, const string& dstWord);
		// 替代字符串中的字符串
		static void ReplaceAllString(string& oriStr, const string& srcStr, const string& dstStr);
		// 获取由sChar和eChar括起来的一段字符串起止位置
		static void GetNextStringBlockPos(string& oriStr, size_t offset, char sChar, char eChar, size_t& sPos, size_t& ePos);
		// 字符串拼接
		static string ConcatenateStrings(const vector<string>& strings);
		// 路径转换为Windows格式
		static inline string ConvertPathToWindowsFormat(const string path);
		// 相对路径转换为绝对路径
		static string RelativePathToAbsolutePath(const string& rPath);
		// 获取文件扩展名
		static inline string GetFileExtension(const string& path);
		// 时间(秒)转换为字符串
		static string SecondsToString(float seconds);
		static string SecondsToString(uint32_t seconds);
		// 时间(毫秒)转换为字符串
		static string MillisecondsToString(uint32_t milliseconds);
		// 数据大小(Byte)转换为字符串
		static string DataSizeToString(uint64_t size);


		// 获取当前可执行文件(ZXEngine)路径
		static string GetCurrentExecutableFilePath();
		// 弹出系统消息框
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