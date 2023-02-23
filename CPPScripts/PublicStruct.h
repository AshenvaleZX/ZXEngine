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
	
	// Shader���м�¼��Ϣ�Ľṹ��
	struct ShaderInfo
	{
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		LightType lightType;
		ShadowType shadowType;
	};

	// ��zxshader�ļ���������������
	struct ShaderData
	{
		string vertexCode;
		string geometryCode;
		string fragmentCode;
		LightType lightType;
		ShadowType shadowType;
	};

	struct ShaderStateSet
	{
		BlendOption blendOp			 = BlendOption::ADD;
		BlendFactor srcFactor		 = BlendFactor::SRC_ALPHA;
		BlendFactor dstFactor		 = BlendFactor::ONE_MINUS_SRC_ALPHA;
		FaceCullOption cull			 = FaceCullOption::Back;
		CompareOption depthCompareOp = CompareOption::LESS;
		bool depthWrite				 = true;
	};
}