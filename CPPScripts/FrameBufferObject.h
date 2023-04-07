#pragma once
#include "pubh.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class FrameBufferObject
	{
	public:
		uint32_t ID = 0;
		uint32_t ColorBuffer = UINT32_MAX;
		uint32_t DepthBuffer = UINT32_MAX;
		bool isFollowWindow = false;
		ClearInfo clearInfo = {};
		FrameBufferType type;

		FrameBufferObject() { this->type = FrameBufferType::Normal; };
		FrameBufferObject(FrameBufferType type) { this->type = type; };
	};
}