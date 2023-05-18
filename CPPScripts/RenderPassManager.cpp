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
		if (ProjectSetting::renderPipelineType == RenderPipelineType::Rasterization)
		{
			passes.push_back(new RenderPassShadowGeneration());
			passes.push_back(new RenderPassForwardRendering());
			passes.push_back(new RenderPassAfterEffectRendering());
			passes.push_back(new RenderPassUIRendering());
		}
		else if (ProjectSetting::renderPipelineType == RenderPipelineType::RayTracing)
		{
			passes.push_back(new RenderPassRayTracing());
			passes.push_back(new RenderPassAfterEffectRendering());
			passes.push_back(new RenderPassUIRendering());
		}
		else
		{
			Debug::LogError("Invalid render pipeline type!");
		}
	}
}