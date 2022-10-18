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

		Material() { shader = new Shader("..\\..\\Shaders\\ZXShaders\\simpleTexture.zxshader"); };
		~Material() {};

		int GetRenderQueue();
	};
}