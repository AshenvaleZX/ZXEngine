#include "RenderQueueManager.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	RenderQueueManager* RenderQueueManager::mInstance = nullptr;

	void RenderQueueManager::Creat()
	{
		mInstance = new RenderQueueManager();
	}

	RenderQueueManager* RenderQueueManager::GetInstance()
	{
		return mInstance;
	}

	RenderQueue* RenderQueueManager::GetRenderQueue(int queue)
	{
		map<int, RenderQueue*>::iterator iter = renderQueues.find(queue);
		if (iter != renderQueues.end()) {
			return iter->second;
		}
		else {
			return nullptr;
		}
	}

	void RenderQueueManager::AddRenderer(MeshRenderer* meshRenderer)
	{
		int queue = meshRenderer->matetrial->GetRenderQueue();
		auto renderQueue = this->GetRenderQueue(queue);
		renderQueue->AddRenderer(meshRenderer);
	}
}