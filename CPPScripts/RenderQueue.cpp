#include "RenderQueue.h"

namespace ZXEngine
{
	void RenderQueue::AddRenderer(MeshRenderer* meshRenderer)
	{
		renderers.push_back(meshRenderer);
	}

	list<MeshRenderer*> RenderQueue::GetRenderers()
	{
		return renderers;
	}

	void RenderQueue::ClearRenderer()
	{
		renderers.clear();
	}
}