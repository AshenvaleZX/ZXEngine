#include "Encryption.h"

namespace ZXEngine
{
	vector<unsigned char> Encryption::XOREncrypt(const vector<unsigned char>& data, const string& key)
	{
		size_t keySize = key.size();
		vector<unsigned char> result(data.size());

		for (size_t i = 0; i < data.size(); i++)
		{
			result[i] = data[i] ^ key[i % keySize];
		}

		return result;
	}

	vector<unsigned char> Encryption::XORDecrypt(const vector<unsigned char>& data, const string& key)
	{
		return XOREncrypt(data, key);
	}

	string Encryption::XOREncryptString(const string& data, const string& key)
	{
		vector<unsigned char> dataVec(data.begin(), data.end());
		vector<unsigned char> result = XOREncrypt(dataVec, key);
		return string(result.begin(), result.end());
	}

	string Encryption::XORDecryptString(const string& data, const string& key)
	{
		return XOREncryptString(data, key);
	}

	vector<unsigned char> Encryption::XOREncryptStream(std::ifstream& ifs, const string& key)
	{
		vector<unsigned char> data((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		return XOREncrypt(data, key);
	}

	vector<unsigned char> Encryption::XORDecryptStream(std::ifstream& ifs, const string& key)
	{
		return XOREncryptStream(ifs, key);
	}

	void Encryption::XOREncryptStream(std::ifstream& ifs, std::ofstream& ofs, const string& key)
	{
		vector<unsigned char> result = XOREncryptStream(ifs, key);
		ofs.write(reinterpret_cast<const char*>(result.data()), result.size());
	}

	void Encryption::XORDecryptStream(std::ifstream& ifs, std::ofstream& ofs, const string& key)
	{
		XOREncryptStream(ifs, ofs, key);
	}

	void Encryption::XOREncryptFile(const string& iPath, const string& oPath, const string& key)
	{
		std::ifstream ifs(iPath, std::ios::binary);
		std::ofstream ofs(oPath, std::ios::binary | std::ios::trunc);
		XOREncryptStream(ifs, ofs, key);
	}

	void Encryption::XORDecryptFile(const string& iPath, const string& oPath, const string& key)
	{
		XOREncryptFile(iPath, oPath, key);
	}
}