#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class RenderPassForwardRendering : public RenderPass
	{
	public:
		RenderPassForwardRendering() {};
		~RenderPassForwardRendering() {};

		virtual void Render();
	};
}