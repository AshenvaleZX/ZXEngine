#pragma once
#include "RenderPass.h"
#include "PublicStruct.h"

namespace ZXEngine
{
	class Material;
	class Camera;
	class StaticMesh;
	class RenderStateSetting;
	class RenderPassForwardRendering : public RenderPass
	{
	public:
		RenderPassForwardRendering();
		~RenderPassForwardRendering() {};

		virtual void Render(Camera* camera);

	private:
		uint32_t drawCommandID = 0;
		StaticMesh* skyBox;
		Material* skyBoxMaterial;
		RenderStateSetting* skyBoxRenderState;
		RenderStateSetting* opaqueRenderState;

		void InitSkyBox();
		void RenderSkyBox(Camera* camera);
	};
}