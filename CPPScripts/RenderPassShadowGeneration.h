#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class RenderPassShadowGeneration : public RenderPass
	{
	public:
		RenderPassShadowGeneration() {};
		~RenderPassShadowGeneration() {};

		virtual void Render();
	};
}