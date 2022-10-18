#include "Texture.h"

namespace ZXEngine
{
	Texture::Texture(const char* path)
	{
		ID = RenderAPI::GetInstance()->LoadTexture(path);
	}

	unsigned int Texture::GetID()
	{
		return ID;
	}
}