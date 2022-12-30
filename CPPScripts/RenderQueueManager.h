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

		void AddGameObject(GameObject* gameObject);
		RenderQueue* GetRenderQueue(int queue);
		void ClearAllRenderQueue();
		list<GameObject*> GetUIGameObjects();
		void ClearUIGameObjects();

	private:
		static RenderQueueManager* mInstance;
		map<int, RenderQueue*> renderQueues = { {(int)RenderQueueType::Qpaque, new RenderQueue()}, {(int)RenderQueueType::Transparent, new RenderQueue()} };
		list<GameObject*> uiGameObjectList;

		void AddRenderer(MeshRenderer* meshRenderer);
		void AddUIGameObject(GameObject* uiGameObject);
	};
}