#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Texture
	{
	public:
		int width = 0;
		int height = 0;
		string path;
		TextureType type;

		Texture(const string& path);
		Texture(const vector<string>& faces);
		Texture(TextureFullData* data);
		Texture(CubeMapFullData* data);
		~Texture();

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}