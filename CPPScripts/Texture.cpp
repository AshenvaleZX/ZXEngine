#include "Texture.h"

namespace ZXEngine
{
	Texture::Texture(const char* path)
	{
		ID = RenderAPI::GetInstance()->LoadTexture(path, width, height);
	}

	unsigned int Texture::GetID()
	{
		return ID;
	}
}