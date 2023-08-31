#pragma once
#include <string>
#include <vector>
#include "Math.h"
#include "PublicEnum.h"

// 顶点最多关联4个骨骼
#define MAX_NUM_BONES_PER_VERTEX 4

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
		// 骨骼蒙皮数据
		float    Weights[MAX_NUM_BONES_PER_VERTEX] = {};
		uint32_t BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {};

		void AddBoneData(uint32_t boneID, float weight)
		{
			for (uint32_t i = 0; i < MAX_NUM_BONES_PER_VERTEX; i++)
			{
				if (Weights[i] == 0.0f)
				{
					BoneIDs[i] = boneID;
					Weights[i] = weight;
					return;
				}
			}
		}
	};

	struct BoneNode
	{
		string name;
		Matrix4 transform;
		vector<BoneNode*> children;
	};

	struct BoneInfo
	{
		Matrix4 offset;
		Matrix4 curTransform;

		BoneInfo(const Matrix4& offset) : offset(offset) {}
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
		RenderQueueType renderQueue  = RenderQueueType::Opaque;
		bool depthWrite              = true;
	};

	struct ShaderProperty
	{
		string name = "";
		uint32_t size = 0;        // 整个属性大小(如果是数组则代表整个数组的大小)
		uint32_t align = 0;       // 单个属性的对齐标准
		uint32_t offset = 0;      // 属性在Uniform Buffer(Vulkan)或Constant Buffer(D3D12)中的偏移量
		uint32_t binding = 0;     // 在Vulkan中代表Uniform Buffer和纹理的layout binding，在D3D12中代表纹理的register(t)索引
		uint32_t arrayLength = 0; // 属性数组长度
		uint32_t arrayOffset = 0; // 如果是数组的话，一个属性在数组内的偏移量
		ShaderPropertyType type = ShaderPropertyType::FLOAT;
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

	struct RayTracingPipelineConstants
	{
		Matrix4 VP;
		Matrix4 V_Inv;
		Matrix4 P_Inv;
		Vector3 lightPos;
		uint32_t frameCount;
	};

	struct RayTracingHitGroupPath
	{
		string rClosestHitPath;
		string rAnyHitPath;
		string rIntersectionPath;
	};

	struct RayTracingShaderPathGroup
	{
		vector<string> rGenPaths;
		vector<string> rMissPaths;
		vector<RayTracingHitGroupPath> rHitGroupPaths;
	};
}