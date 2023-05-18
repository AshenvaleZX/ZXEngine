#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Camera;
	class RenderPassRayTracing : public RenderPass
	{
	public:
		RenderPassRayTracing() {};
		~RenderPassRayTracing() {};

		virtual void Render(Camera* camera);
	};
}