#pragma once
#include "pubh.h"
#include "RenderQueue.h"

namespace ZXEngine
{
	class GameObject;
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
		void AddUIGameObject(GameObject* uiGameObject);
		list<GameObject*> GetUIGameObjects();
		void ClearUIGameObjects();

	private:
		static RenderQueueManager* mInstance;
		map<int, RenderQueue*> renderQueues = { {RenderQueueType::Qpaque, new RenderQueue()}, {RenderQueueType::Transparent, new RenderQueue()} };
		list<GameObject*> uiGameObjectList;
	};
}