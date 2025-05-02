#pragma once
#include "../pubh.h"
#include "../PublicStruct.h"

#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>

namespace ZXEngine
{
	constexpr uint32_t MT_MAX_FRAMES_IN_FLIGHT = 2;

	struct MetalBuffer
	{
		MTL::Buffer* buffer = nullptr;
		bool inUse = false;
	};

	struct MetalTexture
	{
		MTL::Texture* texture = nullptr;
		bool inUse = false;
	};

	struct MetalVAO
	{
		uint32_t indexCount = 0; // 索引数量
		MTL::Buffer* indexBuffer = nullptr;

		uint32_t vertexCount = 0; // 顶点数量
		MTL::Buffer* vertexBuffer = nullptr;

		bool computeSkinned = false;
		vector<MTL::Buffer*> ssbo; // For compute pipeline

		bool inUse = false;
	};

	struct MetalPipeline
	{
        string name; // For debug
		MTL::RenderPipelineState* pipeline = nullptr;
		MTL::DepthStencilState* depthStencilState = nullptr;
		FaceCullOption faceCullOption = FaceCullOption::None;
		bool inUse = false;
	};

	struct MetalComputePipeline
	{
		MTL::ComputePipelineState* pipeline = nullptr;
		bool inUse = false;
	};

	struct MetalMaterialData
	{
		uint32_t size = 0; // For debug
		vector<MTL::Buffer*> constantBuffers;
		vector<unordered_map<uint32_t, uint32_t>> textures;
		bool inUse = false;
	};

	// Metal的MTL::CommandBuffer是一个一次性对象，没必要也不可以重复使用，所以这里没有CommandBuffer
	struct MetalDrawCommand
	{
		CommandType commandType = CommandType::NotCare;
		FrameBufferClearFlags clearFlags = ZX_CLEAR_FRAME_BUFFER_NONE_BIT;
		bool inUse = false;
	};

	struct MetalRenderBuffer
	{
		vector<uint32_t> renderBuffers;
		bool inUse = false;
	};

	struct MetalFBO
	{
		uint32_t colorBufferIdx = UINT32_MAX;
		uint32_t depthBufferIdx = UINT32_MAX;
		uint32_t positionBufferIdx = UINT32_MAX; // GBuffer
		uint32_t normalBufferIdx   = UINT32_MAX; // GBuffer
		FrameBufferType bufferType = FrameBufferType::Normal;
		ClearInfo clearInfo = {};
		MTL::RenderPassDescriptor* renderPassDescriptor = nullptr;
		bool inUse = false;
	};

    struct MetalDrawRecord
    {
        uint32_t VAO = 0;
        uint32_t pipelineID = 0;
        uint32_t materialDataID = 0;
        uint32_t instanceNum = 0;
        uint32_t instanceBuffer = UINT32_MAX;

        // 必须显示声明构造函数，否则在MacOS上无法使用emplace操作在一些容器中直接构造
        MetalDrawRecord(uint32_t vao, uint32_t pipelineID, uint32_t materialDataID, uint32_t instanceNum, uint32_t instanceBuffer) :
            VAO(vao), pipelineID(pipelineID), materialDataID(materialDataID), instanceNum(instanceNum), instanceBuffer(instanceBuffer)
        {}
    };
}