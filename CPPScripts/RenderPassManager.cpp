#include "RenderPassManager.h"
#include "RenderPassForwardRendering.h"
#include "RenderPassShadowGeneration.h"

namespace ZXEngine
{
	RenderPassManager::RenderPassManager()
	{
		passes.push_back(new RenderPassShadowGeneration());
		passes.push_back(new RenderPassForwardRendering());
	}
}