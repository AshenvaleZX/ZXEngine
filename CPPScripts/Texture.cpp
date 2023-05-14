#include "Texture.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	Texture::Texture(const char* path)
	{
		type = TextureType::ZX_2D;
		ID = RenderAPI::GetInstance()->LoadTexture(path, width, height);
	}

	Texture::Texture(const vector<string>& faces)
	{
		type = TextureType::ZX_Cube;
		ID = RenderAPI::GetInstance()->LoadCubeMap(faces);
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