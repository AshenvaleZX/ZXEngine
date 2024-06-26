#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Material;
	class Camera;
	class StaticMesh;
	class MeshRenderer;
	class RenderStateSetting;
	class RenderPassForwardRendering : public RenderPass
	{
	public:
		RenderPassForwardRendering();
		~RenderPassForwardRendering() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t drawCommandID = 0;
		uint32_t blitCommandID = 0;
		StaticMesh* skyBox;
		Material* skyBoxMaterial;
		RenderStateSetting* skyBoxRenderState;
		RenderStateSetting* opaqueRenderState;
		RenderStateSetting* transparentRenderState;

		void RenderSkyBox(Camera* camera);
		void RenderBatches(const map<uint32_t, vector<MeshRenderer*>>& batchs);
	};
}