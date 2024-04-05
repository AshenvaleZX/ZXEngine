#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Shader;
	class Material;
	class StaticMesh;
	class MeshRenderer;
	class RenderStateSetting;
	class RenderPassDeferredRendering : public RenderPass
	{
	public:
		RenderPassDeferredRendering();
		~RenderPassDeferredRendering() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t mBlitCommandID = 0;
		uint32_t mDrawCommandID = 0;
		StaticMesh* mScreenQuad;

		Shader* mDeferredShader;
		Material* mDeferredMaterial;

		RenderStateSetting* opaqueRenderState;
		RenderStateSetting* deferredRenderState;

		void RenderBatches(const map<uint32_t, vector<MeshRenderer*>>& batchs);
	};
}