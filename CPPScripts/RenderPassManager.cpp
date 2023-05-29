#include "RenderPassManager.h"
#include "ProjectSetting.h"
#include "RenderPassForwardRendering.h"
#include "RenderPassShadowGeneration.h"
#include "RenderPassAfterEffectRendering.h"
#include "RenderPassUIRendering.h"
#include "RenderPassRayTracing.h"

namespace ZXEngine
{
	RenderPassManager::RenderPassManager()
	{
		allPasses.resize(ZX_RENDER_PASS_COUNT);

		allPasses[ZX_RENDER_PASS_SHADOW_GENERATION]      = new RenderPassShadowGeneration();
		allPasses[ZX_RENDER_PASS_FORWARD_RENDERING]      = new RenderPassForwardRendering();
		allPasses[ZX_RENDER_PASS_RAY_TRACING]            = new RenderPassRayTracing();
		allPasses[ZX_RENDER_PASS_AFTER_EFFECT_RENDERING] = new RenderPassAfterEffectRendering();
		allPasses[ZX_RENDER_PASS_UI_RENDERING]           = new RenderPassUIRendering();

		SetUpRenderPasses();
	}

	RenderPassManager* RenderPassManager::mInstance = nullptr;

	void RenderPassManager::Create()
	{
		mInstance = new RenderPassManager();
	}

	RenderPassManager* RenderPassManager::GetInstance()
	{
		return mInstance;
	}

	void RenderPassManager::SetUpRenderPasses()
	{
		curPasses.clear();

		if (ProjectSetting::renderPipelineType == RenderPipelineType::Rasterization)
		{
			curPasses.push_back(allPasses[ZX_RENDER_PASS_SHADOW_GENERATION]);
			curPasses.push_back(allPasses[ZX_RENDER_PASS_FORWARD_RENDERING]);
			curPasses.push_back(allPasses[ZX_RENDER_PASS_AFTER_EFFECT_RENDERING]);
			curPasses.push_back(allPasses[ZX_RENDER_PASS_UI_RENDERING]);
		}
		else if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
		{
			curPasses.push_back(allPasses[ZX_RENDER_PASS_RAY_TRACING]);
			curPasses.push_back(allPasses[ZX_RENDER_PASS_AFTER_EFFECT_RENDERING]);
			curPasses.push_back(allPasses[ZX_RENDER_PASS_UI_RENDERING]);
		}
		else
		{
			Debug::LogError("Invalid render pipeline type!");
		}
	}
}