#pragma once
#include "pubh.h"

#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

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
		
		bool inUse = false;
	};

	struct MetalPipeline
	{
		MTL::RenderPipelineState* pipeline = nullptr;
		MTL::DepthStencilState* depthStencilState = nullptr;
		FaceCullOption faceCullOption = FaceCullOption::None;
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
}