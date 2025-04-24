#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include "Math.h"
#include "PublicEnum.h"
#include "Debug.h"

// 一个Mesh支持的最大骨骼数量
constexpr uint32_t MAX_BONE_NUM = 70;

// 顶点最多关联4个骨骼
constexpr size_t MAX_NUM_BONES_PER_VERTEX = 4;

using std::string;
using std::vector;
using std::shared_ptr;

namespace ZXEngine
{
	struct Vertex 
	{
		Vector4 Position  = {};
		Vector4 TexCoords = {};
		Vector4 Normal    = {};
		Vector4 Tangent   = {};
		// 骨骼蒙皮数据
		float    Weights[MAX_NUM_BONES_PER_VERTEX] = {};
		uint32_t BoneIDs[MAX_NUM_BONES_PER_VERTEX] = {};

		void AddBoneData(uint32_t boneID, float weight);
		void NormalizeWeights();
	};

	struct BoneNode
	{
		string name;
		Matrix4 transform;
		vector<BoneNode*> children;

		~BoneNode();
	};

	struct AnimBriefInfo
	{
		string name;
		float duration = 0.0f;
	};

	struct TextureFullData
	{
		int width = 0;
		int height = 0;
		int numChannel = 0;
		unsigned char* data = nullptr;
		string path;

		~TextureFullData();
	};

	struct CubeMapFullData
	{
		int width = 0;
		int height = 0;
		int numChannel = 0;
		unsigned char* data[6] = {};

		~CubeMapFullData();
	};

	class Mesh;
	class AnimationController;
	struct ModelData
	{
		vector<shared_ptr<Mesh>> pMeshes;
		uint32_t boneNum = 0;
		BoneNode* pRootBoneNode = nullptr;
		AnimationController* pAnimationController = nullptr;
		vector<AnimBriefInfo> animBriefInfos;
		bool isConstructed = false;
	};

	struct KeyFrame
	{
		Vector3 mScale;
		Vector3 mPosition;
		Quaternion mRotation;
	};

	struct ClearInfo
	{
		Vector4 color    = { 0.0f, 0.0f, 0.0f, 1.0f };
		float depth      = 1.0f;
		uint32_t stencil = 0;
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
		uint32_t binding = 0;     // 在Vulkan中代表Uniform Buffer和纹理的layout binding，在D3D12中代表纹理的register(t)索引，在Metal中代表纹理的[[texture(n)]]索引
		uint32_t arrayLength = 0; // 属性数组长度
		uint32_t arrayOffset = 0; // 如果是数组的话，一个属性在数组内的偏移量
		ShaderPropertyType type = ShaderPropertyType::FLOAT;
	};

	struct ShaderPropertiesInfo
	{
		vector<ShaderProperty> baseProperties;
		vector<ShaderProperty> textureProperties;
	};

	using ShaderSemanticType = std::pair<ShaderPropertyType, string>;
	struct ShaderInstanceInfo
	{
		uint32_t size = 0; // 以Vector4为单位
		vector<ShaderSemanticType> attributes;
	};

	// Shader类中记录信息的结构体
	struct ShaderInfo
	{
		LightType lightType   = LightType::None;
		ShadowType shadowType = ShadowType::None;
		ShaderStateSet stateSet;
		ShaderStageFlags stages = 0;
		ShaderInstanceInfo instanceInfo;
		ShaderPropertiesInfo vertProperties;
		ShaderPropertiesInfo geomProperties;
		ShaderPropertiesInfo fragProperties;
	};

	struct ShaderBufferInfo
	{
		uint32_t binding = 0;
		bool isReadOnly = true;
		ShaderBufferType type = ShaderBufferType::Uniform;
	};

	struct ComputeShaderInfo
	{
		vector<ShaderBufferInfo> bufferInfos;
	};

	struct ShaderReference
	{
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		FrameBufferType targetFrameBufferType = FrameBufferType::Color;
		ShaderInfo shaderInfo;
	};

	struct ComputeShaderReference
	{
		string path;
		unsigned int ID = 0;
		int referenceCount = 1;
		ComputeShaderInfo shaderInfo;
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
		float time;
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

	static const std::multimap<ComponentType, ComponentType> ComponentChildLUT
	{
		{ ComponentType::Collider,  ComponentType::BoxCollider      },
		{ ComponentType::Collider,  ComponentType::PlaneCollider    },
		{ ComponentType::Collider,  ComponentType::SphereCollider   },
		{ ComponentType::Collider,  ComponentType::Circle2DCollider },
		{ ComponentType::Transform, ComponentType::RectTransform    },
	};

	static const std::unordered_map<ComponentType, ComponentType> ComponentParentLUT
	{
		{ ComponentType::BoxCollider,      ComponentType::Collider  },
		{ ComponentType::PlaneCollider,    ComponentType::Collider  },
		{ ComponentType::SphereCollider,   ComponentType::Collider  },
		{ ComponentType::Circle2DCollider, ComponentType::Collider  },
		{ ComponentType::RectTransform,    ComponentType::Transform },
	};
}