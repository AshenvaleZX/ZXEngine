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

	struct ShaderProperty
	{
		string name = "";
		uint32_t size = 0;        // �������Դ�С(����������������������Ĵ�С)
		uint32_t align = 0;       // �������ԵĶ����׼
		uint32_t offset = 0;      // ������Uniform Buffer(Vulkan)��Constant Buffer(D3D12)�е�ƫ����
		uint32_t binding = 0;     // ��Vulkan�д���Uniform Buffer�������layout binding����D3D12�д��������register(t)����
		uint32_t arrayLength = 0; // �������鳤��
		uint32_t arrayOffset = 0; // ���������Ļ���һ�������������ڵ�ƫ����
		ShaderPropertyType type;
	};

	struct ShaderPropertiesInfo
	{
		vector<ShaderProperty> baseProperties;
		vector<ShaderProperty> textureProperties;
	};

	// Shader���м�¼��Ϣ�Ľṹ��
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

	struct RayTracingPipelineConstants
	{
		Matrix4 VP;
		Matrix4 V_Inv;
		Matrix4 P_Inv;
		Vector3 lightPos;
		uint32_t frameCount;
	};

	struct RayTracingShaderPathGroup
	{
		vector<string> rGenPaths;
		vector<string> rMissPaths;
		vector<string> rClosestHitPaths;
		vector<string> rAnyHitPaths;
		vector<string> rIntersectionPaths;
	};
}