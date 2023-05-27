#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Camera;
	class RenderPassRayTracing : public RenderPass
	{
	public:
		RenderPassRayTracing();
		~RenderPassRayTracing() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t asCommandID;
		uint32_t rtCommandID;
		RayTracingPipelineConstants rtConstants;
	};
}