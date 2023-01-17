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
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		LightType lightType;
		ShadowType shadowType;
	};
}