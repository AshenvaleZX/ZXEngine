#include "Texture.h"

namespace ZXEngine
{
	Texture::Texture(const char* path)
	{
		ID = RenderEngine::LoadTexture(path);
	}

	unsigned int Texture::GetID()
	{
		return ID;
	}
}