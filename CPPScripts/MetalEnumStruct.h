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

	struct MetalPipeline
	{
		MTL::RenderPipelineState* pipeline = nullptr;
		MTL::DepthStencilState* depthStencilState = nullptr;
		FaceCullOption faceCullOption = FaceCullOption::None;
		bool inUse = false;
	};
}