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
		mat4 shadowProj;
		vector<mat4> shadowTransforms;
		Shader* shadowCubeMapShader;

		void RenderShadowMap(Light* light);
		void RenderShadowCubeMap(Light* light);
	};
}