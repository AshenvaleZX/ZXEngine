#pragma once
#include "pubh.h"
#include "Texture.h"
#include "Resources.h"

namespace ZXEngine
{
	class Shader;
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