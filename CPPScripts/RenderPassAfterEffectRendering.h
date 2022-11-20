#pragma once
#include "RenderPass.h"

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class Camera;
	class RenderPassAfterEffectRendering : public RenderPass
	{
	public:
		Shader* aeShader;

		RenderPassAfterEffectRendering();
		~RenderPassAfterEffectRendering() {};

		virtual void Render(Camera* camera);

	private:
		Mesh* screenQuad;

		void InitScreenQuad();
	};
}