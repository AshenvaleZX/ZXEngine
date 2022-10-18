#include "RenderQueue.h"

namespace ZXEngine
{
	void RenderQueue::AddRenderer(MeshRenderer* meshRenderer)
	{
		renderers.push_back(meshRenderer);
	}
}