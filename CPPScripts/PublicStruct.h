#pragma once
#include "PublicEnum.h"

namespace ZXEngine
{
	struct Vertex {
		Vector3 Position;
		Vector3 Normal;
		Vector2 TexCoords;
		Vector3 Tangent;
		Vector3 Bitangent;
	};
	
	struct ShaderInfo
	{
		unsigned int ID;
		LightType lightType;
		ShadowType shadowType;
	};
}