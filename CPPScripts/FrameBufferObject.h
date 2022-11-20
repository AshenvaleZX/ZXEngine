#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class FrameBufferObject
	{
	public:
		unsigned int ID = 0;
		unsigned int ColorBuffer = 0;
		unsigned int DepthBuffer = 0;
		FrameBufferType type;
	};
}