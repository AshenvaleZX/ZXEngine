#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Shader;
	class RenderPassGBufferGeneration : public RenderPass
	{
	public:
		RenderPassGBufferGeneration();
		~RenderPassGBufferGeneration() {};

		virtual void Render(Camera* camera);

	private:
		Shader* mGBufferShader;
		uint32_t mDrawCommandID = 0;
	};
}