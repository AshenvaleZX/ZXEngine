#pragma once
#include "pubh.h"

namespace ZXEngine
{
	enum RenderPassType
	{
		ZX_RENDER_PASS_SHADOW_GENERATION,
		ZX_RENDER_PASS_FORWARD_RENDERING,
		ZX_RENDER_PASS_GBUFFER_GENERATION,
		ZX_RENDER_PASS_DEFERRED_RENDERING,
		ZX_RENDER_PASS_RAY_TRACING,
		ZX_RENDER_PASS_AFTER_EFFECT_RENDERING,
		ZX_RENDER_PASS_UI_RENDERING,
		ZX_RENDER_PASS_COUNT
	};

	class RenderPass;
	class RenderPassManager
	{
	public:
		static void Create();
		static RenderPassManager* GetInstance();

	private:
		static RenderPassManager* mInstance;

	public:
		vector<RenderPass*> curPasses;
		vector<RenderPass*> allPasses;

		RenderPassManager();
		~RenderPassManager() {};

		void SetUpRenderPasses();
	};
}