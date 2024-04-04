#pragma once
#include "pubh.h"
#include "RenderQueue.h"

namespace ZXEngine
{
	class GameObject;
	class RenderQueueManager
	{
	public:
		static void Creat();
		static RenderQueueManager* GetInstance();

	public:
		RenderQueueManager();
		~RenderQueueManager();

		void AddGameObject(GameObject* gameObject);
		RenderQueue* GetRenderQueue(int queue);
		void ClearAllRenderQueue();
		list<GameObject*> GetUIGameObjects();
		void ClearUIGameObjects();

	private:
		static RenderQueueManager* mInstance;
		unordered_map<int, RenderQueue*> renderQueues;
		list<GameObject*> uiGameObjectList;

		void AddRenderer(MeshRenderer* meshRenderer);
		void AddUIGameObject(GameObject* uiGameObject);
	};
}