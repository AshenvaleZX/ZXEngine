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

	struct ShaderStateSet
	{
		BlendOption blendOp          = BlendOption::ADD;
		BlendFactor srcFactor        = BlendFactor::SRC_ALPHA;
		BlendFactor dstFactor        = BlendFactor::ONE_MINUS_SRC_ALPHA;
		FaceCullOption cull          = FaceCullOption::Back;
		CompareOption depthCompareOp = CompareOption::LESS;
		bool depthWrite              = true;
	};

	// Shader类中记录信息的结构体
	struct ShaderInfo
	{
		LightType lightType   = LightType::None;
		ShadowType shadowType = ShadowType::None;
		ShaderStateSet stateSet;
	};

	struct ShaderReference
	{
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		ShaderInfo shaderInfo;
	};
}