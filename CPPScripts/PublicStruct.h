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
	
	// Shader类中记录信息的结构体
	struct ShaderInfo
	{
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		LightType lightType;
		ShadowType shadowType;
	};

	// 从zxshader文件解析出来的数据
	struct ShaderData
	{
		string vertexCode;
		string geometryCode;
		string fragmentCode;
		LightType lightType;
		ShadowType shadowType;
	};
}