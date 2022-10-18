#pragma once
#include "pubh.h"
#include "PublicEnum.h"
#include "RenderQueue.h"

namespace ZXEngine
{
	class RenderQueueManager
	{
	public:
		RenderQueueManager() {};
		~RenderQueueManager() {};

		static void Creat();
		static RenderQueueManager* GetInstance();

		RenderQueue* GetRenderQueue(int queue);
		void AddRenderer(MeshRenderer* meshRenderer);
		void ClearAllRenderQueue();

	private:
		static RenderQueueManager* mInstance;
		map<int, RenderQueue*> renderQueues = { {RenderQueueType::Qpaque, new RenderQueue()}, {RenderQueueType::Transparent, new RenderQueue()} };
	};
}