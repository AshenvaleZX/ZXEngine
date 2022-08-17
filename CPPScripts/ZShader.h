#pragma once
#include "RenderEngine.h"

namespace ZXEngine
{
	class Shader
	{
	public:
		Shader(const char* path);
		~Shader() {};

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}