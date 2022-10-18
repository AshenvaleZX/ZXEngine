#pragma once
#include "pubh.h"
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

		map<int, RenderQueue*> renderQueues;
		RenderQueue* GetRenderQueue(int queue);
		void AddRenderer(MeshRenderer* meshRenderer);

	private:
		static RenderQueueManager* mInstance;
	};
}