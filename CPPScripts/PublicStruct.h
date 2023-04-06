#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include "PublicEnum.h"

using std::string;
using std::vector;

namespace ZXEngine
{
	struct Vertex 
	{
		Vector3 Position  = {};
		Vector2 TexCoords = {};
		Vector3 Normal    = {};
		Vector3 Tangent   = {};
		Vector3 Bitangent = {};
	};

	struct ClearInfo
	{
		FrameBufferClearFlags clearFlags = ZX_CLEAR_FRAME_BUFFER_NONE_BIT;
		Vector4 color                    = {};
		float depth                      = 1.0f;
		uint32_t stencil                 = 0;
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

	struct UniformAlignInfo
	{
		uint32_t size = 0;
		uint32_t align = 0;
		uint32_t arrayOffset = 0;
	};

	struct ShaderProperty
	{
		string name = "";
		uint32_t size = 0;        // 整个属性大小(可能是数组)
		uint32_t align = 0;       // 单个属性的对齐标准
		uint32_t offset = 0;      // Offset in uniform buffer, Only for Vulkan now
		uint32_t binding = 0;     // Only for Vulkan now
		uint32_t arrayLength = 0; // 属性数组长度
		uint32_t arrayOffset = 0; // 如果是数组的话，一个属性在数组内的偏移量
		ShaderPropertyType type;
	};

	struct ShaderPropertiesInfo
	{
		vector<ShaderProperty> baseProperties;
		vector<ShaderProperty> textureProperties;
	};

	// Shader类中记录信息的结构体
	struct ShaderInfo
	{
		LightType lightType   = LightType::None;
		ShadowType shadowType = ShadowType::None;
		ShaderStateSet stateSet;
		ShaderStageFlags stages = 0;
		ShaderPropertiesInfo vertProperties;
		ShaderPropertiesInfo geomProperties;
		ShaderPropertiesInfo fragProperties;
	};

	struct ShaderReference
	{
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		ShaderInfo shaderInfo;
	};

	struct ViewPortInfo
	{
		uint32_t width = 0;
		uint32_t height = 0;
		int32_t xOffset = 0;
		int32_t yOffset = 0;
	};
}