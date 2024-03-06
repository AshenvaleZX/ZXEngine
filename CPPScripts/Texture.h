#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Texture
	{
	public:
		int width;
		int height;
		TextureType type;

		Texture(const char* path);
		Texture(const vector<string>& faces);
		Texture(TextureFullData* data);
		Texture(CubeMapFullData* data);
		~Texture();

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}