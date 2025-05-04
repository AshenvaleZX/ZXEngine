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
		vector<uint32_t> drawCommandIDs;
		Matrix4 shadowProj;
		vector<Matrix4> shadowTransforms;

		Shader* shadowMapShader = nullptr;
		Shader* animShadowMapShader = nullptr;
		Shader* shadowCubeMapShader = nullptr;
		Shader* animShadowCubeMapShader = nullptr;
		Shader* shadowCubeMapShaderNonGS = nullptr;
		Shader* animShadowCubeMapShaderNonGS = nullptr;

		RenderStateSetting* renderState = nullptr;

		void RenderShadowMap(Light* light);
		void RenderShadowCubeMap(Light* light);
		void RenderShadowCubeMapWithGS(const Vector3& lightPos);
		void RenderShadowCubeMapWithoutGS(const Vector3& lightPos);
	};
}