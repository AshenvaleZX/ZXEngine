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
		Matrix4 shadowProj;
		vector<Matrix4> shadowTransforms;
		Shader* shadowCubeMapShader;

		void RenderShadowMap(Light* light);
		void RenderShadowCubeMap(Light* light);
	};
}