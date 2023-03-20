#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Light;
	class Shader;
	class RenderStateSetting;
	class RenderPassShadowGeneration : public RenderPass
	{
	public:
		RenderPassShadowGeneration();
		~RenderPassShadowGeneration() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t drawCommandID = 0;
		Matrix4 shadowProj;
		vector<Matrix4> shadowTransforms;
		Shader* shadowCubeMapShader;
		RenderStateSetting* renderState;
		ClearInfo clearInfo;

		void RenderShadowMap(Light* light);
		void RenderShadowCubeMap(Light* light);
	};
}