#pragma once
#include "PublicEnum.h"

namespace ZXEngine
{
	struct Vertex {
		vec3 Position;
		vec3 Normal;
		vec2 TexCoords;
		vec3 Tangent;
		vec3 Bitangent;
	};
	
	struct ShaderInfo
	{
		unsigned int ID;
		LightType lightType;
	};
}