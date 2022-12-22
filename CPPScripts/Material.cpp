#include "Material.h"

namespace ZXEngine
{
	Material::Material(MaterialStruct* matStruct)
	{
		name = matStruct->name;
		shader = new Shader(matStruct->shaderPath.c_str());

		for (auto textureStruct : matStruct->textures)
		{
			Texture* texture = new Texture(textureStruct->path.c_str());
			textures.push_back(make_pair(textureStruct->uniformName, texture));
		}
	}

	Material::~Material()
	{
		delete shader;
		for (auto& iter : textures)
			delete iter.second;
	}

	int Material::GetRenderQueue()
	{
		return shader->GetRenderQueue();
	}
}