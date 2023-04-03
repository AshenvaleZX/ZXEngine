#include "Texture.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	Texture::Texture(const char* path)
	{
		ID = RenderAPI::GetInstance()->LoadTexture(path, width, height);
	}

	Texture::~Texture()
	{
		RenderAPI::GetInstance()->DeleteTexture(ID);
	}

	unsigned int Texture::GetID()
	{
		return ID;
	}
}