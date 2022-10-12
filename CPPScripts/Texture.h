#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class Texture
	{
	public:
		Texture(const char* path);
		~Texture() {};

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}