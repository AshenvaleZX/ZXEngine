#pragma once
#include "ZShader.h"
#include "Texture.h"
#include "Resources.h"

namespace ZXEngine
{
	class Material
	{
	public:
		string name;
		string path;
		Shader* shader = nullptr;
		vector<pair<string, Texture*>> textures;

		Material() {};
		Material(MaterialStruct* matStruct);
		~Material();

		int GetRenderQueue();
	};
}