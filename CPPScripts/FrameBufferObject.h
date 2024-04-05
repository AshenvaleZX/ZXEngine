#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class FrameBufferObject
	{
	public:
		uint32_t ID = 0;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t ColorBuffer = UINT32_MAX;
		uint32_t DepthBuffer = UINT32_MAX;
		uint32_t PositionBuffer = UINT32_MAX; // GBuffer
		uint32_t NormalBuffer   = UINT32_MAX; // GBuffer

		bool isFollowWindow = false;
		ClearInfo clearInfo = {};
		FrameBufferType type;

		FrameBufferObject() { this->type = FrameBufferType::Normal; };
		FrameBufferObject(FrameBufferType type) { this->type = type; };
	};
}