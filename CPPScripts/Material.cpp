#include "Material.h"
#include "ZShader.h"

namespace ZXEngine
{
	Material::Material(MaterialStruct* matStruct)
	{
		name = matStruct->name;
		path = matStruct->path;
		shader = new Shader(matStruct->shaderPath, FrameBufferType::Normal);

		for (auto textureStruct : matStruct->textures)
		{
			Texture* texture = new Texture(textureStruct->path.c_str());
			textures.push_back(make_pair(textureStruct->uniformName, texture));
		}

		map<string, uint32_t> textureIDs;
		for (auto& texture : textures)
			textureIDs.insert(pair(texture.first, static_cast<uint32_t>(texture.second->GetID())));

		RenderAPI::GetInstance()->SetUpMaterial(shader->reference, textureIDs);
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