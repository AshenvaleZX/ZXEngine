#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderStateSetting
	{
	public:
		bool depthTest = true;
		bool depthWrite = true;
		BlendOption srcFactor = BlendOption::SRC_ALPHA;
		BlendOption dstFactor = BlendOption::ONE_MINUS_SRC_ALPHA;
		Vector4 clearColor = Vector4();
		float clearDepth = 1.0f;
		int clearStencil = 0;
		bool faceCull = true;
		FaceCullOption faceCullOption = FaceCullOption::Back;
	};
}