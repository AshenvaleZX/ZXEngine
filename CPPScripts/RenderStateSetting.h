#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderStateSetting
	{
	public:
		bool depthTest = true;
		bool depthWrite = true;
		BlendFactor srcFactor = BlendFactor::SRC_ALPHA;
		BlendFactor dstFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
		Vector4 clearColor = Vector4();
		float clearDepth = 1.0f;
		uint32_t clearStencil = 0;
		bool faceCull = true;
		FaceCullOption faceCullOption = FaceCullOption::Back;
	};
}