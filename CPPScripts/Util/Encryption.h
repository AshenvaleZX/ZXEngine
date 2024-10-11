#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class Encryption
	{
	public:
		static vector<unsigned char> XOREncrypt(const vector<unsigned char>& data, const string& key);
		static vector<unsigned char> XORDecrypt(const vector<unsigned char>& data, const string& key);
		static string XOREncryptString(const string& data, const string& key);
		static string XORDecryptString(const string& data, const string& key);
		static vector<unsigned char> XOREncryptStream(std::ifstream& ifs, const string& key);
		static vector<unsigned char> XORDecryptStream(std::ifstream& ifs, const string& key);
		static void XOREncryptStream(std::ifstream& ifs, std::ofstream& ofs, const string& key);
		static void XORDecryptStream(std::ifstream& ifs, std::ofstream& ofs, const string& key);
		static void XOREncryptFile(const string& iPath, const string& oPath, const string& key);
		static void XORDecryptFile(const string& iPath, const string& oPath, const string& key);
	};
}