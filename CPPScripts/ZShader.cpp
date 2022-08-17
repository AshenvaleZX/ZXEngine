#include "ZShader.h"

namespace ZXEngine
{
	Shader::Shader(const char* path)
	{
		ID = RenderEngine::LoadAndCompileShader(path);
	}

	unsigned int Shader::GetID()
	{
		return ID;
	}
}