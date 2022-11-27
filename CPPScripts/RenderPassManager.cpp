#include "RenderPassManager.h"
#include "RenderPassForwardRendering.h"
#include "RenderPassShadowGeneration.h"
#include "RenderPassAfterEffectRendering.h"
#include "RenderPassUIRendering.h"

namespace ZXEngine
{
	RenderPassManager::RenderPassManager()
	{
		passes.push_back(new RenderPassShadowGeneration());
		passes.push_back(new RenderPassForwardRendering());
		passes.push_back(new RenderPassAfterEffectRendering());
		passes.push_back(new RenderPassUIRendering());
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

}