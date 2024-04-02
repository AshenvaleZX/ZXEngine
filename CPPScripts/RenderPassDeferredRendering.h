#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Shader;
	class Material;
	class StaticMesh;
	class MeshRenderer;
	class RenderPassDeferredRendering : public RenderPass
	{
	public:
		RenderPassDeferredRendering();
		~RenderPassDeferredRendering() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t mDrawCommandID = 0;
		StaticMesh* mScreenQuad;

		Shader* mDeferredShader;
		Material* mDeferredMaterial;
	};
}