#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class RenderPass;
	class RenderPassManager
	{
	public:
		RenderPassManager();
		~RenderPassManager() {};

		vector<RenderPass*> passes;
	};
}