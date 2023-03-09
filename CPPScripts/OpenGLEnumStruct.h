#pragma once
#include "pubh.h"

namespace ZXEngine
{
	struct OpenGLVAO
	{
		// Vertex Array Object
		uint32_t VAO = 0;
		// Vertex Buffer Objects
		uint32_t VBO = 0;
		// Element Buffer Objects
		uint32_t EBO = 0;
		bool inUse = false;
	};
}