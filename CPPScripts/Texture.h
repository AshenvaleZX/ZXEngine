#pragma once
#include "pubh.h"

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
		~Texture();

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}