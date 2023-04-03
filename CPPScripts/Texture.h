#pragma once

namespace ZXEngine
{
	class Texture
	{
	public:
		int width;
		int height;

		Texture(const char* path);
		~Texture();

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}