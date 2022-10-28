#include "Material.h"

namespace ZXEngine
{
	Material::Material(MaterialStruct* matStruct)
	{
		shader = new Shader(matStruct->shaderPath.c_str());
	}

	int Material::GetRenderQueue()
	{
		return shader->GetRenderQueue();
	}
}