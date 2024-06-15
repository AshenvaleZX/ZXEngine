#include "Texture.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	Texture::Texture(const string& path)
	{
		type = TextureType::ZX_2D;
		this->path = path;
		ID = RenderAPI::GetInstance()->LoadTexture(path.c_str(), width, height);
	}

	Texture::Texture(const vector<string>& faces)
	{
		type = TextureType::ZX_Cube;
		ID = RenderAPI::GetInstance()->LoadCubeMap(faces);
	}

	Texture::Texture(TextureFullData* data)
	{
		type = TextureType::ZX_2D;
		width = data->width;
		height = data->height;
		ID = RenderAPI::GetInstance()->CreateTexture(data);
	}

	Texture::Texture(CubeMapFullData* data)
	{
		type = TextureType::ZX_Cube;
		ID = RenderAPI::GetInstance()->CreateCubeMap(data);
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