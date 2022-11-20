#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class Camera;
	class RenderPassForwardRendering : public RenderPass
	{
	public:
		RenderPassForwardRendering();
		~RenderPassForwardRendering() {};

		virtual void Render(Camera* camera);

	private:
		Mesh* skyBox;
		Shader* skyBoxShader;

		void InitSkyBox();
		void RenderSkyBox(Camera* camera);
	};
}