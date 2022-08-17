#pragma once
#include "ZShader.h"
#include "Texture.h"

namespace ZXEngine
{
	class Material
	{
	public:
		Shader* shader = nullptr;
		vector<Texture*> textures;

		Material() {};
		~Material() {};
	};
}