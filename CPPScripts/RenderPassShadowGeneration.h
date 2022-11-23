#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Light;
	class Shader;
	class RenderPassShadowGeneration : public RenderPass
	{
	public:
		RenderPassShadowGeneration();
		~RenderPassShadowGeneration() {};

		virtual void Render(Camera* camera);

	private:
		const unsigned int DEPTH_CUBEMAP_WIDTH = 1024;
		float nearPlane = 1.0f;
		float farPlane = 100.0f;
		mat4 shadowProj;
		vector<mat4> shadowTransforms;
		Shader* shadowCubeMapShader;

		void RenderShadowMap(Light* light);
		void RenderShadowCubeMap(Light* light);
	};
}