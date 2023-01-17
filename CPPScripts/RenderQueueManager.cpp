#include "RenderQueueManager.h"
#include "MeshRenderer.h"
#include "GameObject.h"

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

	void RenderQueueManager::AddGameObject(GameObject* gameObject)
	{
		if (gameObject->layer == (int)GameObjectLayer::UI)
		{
			AddUIGameObject(gameObject);
		}
		else
		{
			MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer != nullptr)
			{
				AddRenderer(gameObject->GetComponent<MeshRenderer>());
			}
		}

		for (auto subGameObject : gameObject->children)
			AddGameObject(subGameObject);
	}

	void RenderQueueManager::AddRenderer(MeshRenderer* meshRenderer)
	{
		int queue = meshRenderer->matetrial->GetRenderQueue();
		auto renderQueue = this->GetRenderQueue(queue);
		renderQueue->AddRenderer(meshRenderer);
	}

	void RenderQueueManager::ClearAllRenderQueue()
	{
		for (auto& iter : renderQueues)
		{
			iter.second->Clear();
		}
	}

	void RenderQueueManager::AddUIGameObject(GameObject* uiGameObject)
	{
		uiGameObjectList.push_back(uiGameObject);
	}

	list<GameObject*> RenderQueueManager::GetUIGameObjects()
	{
		return uiGameObjectList;
	}

	void RenderQueueManager::ClearUIGameObjects()
	{
		uiGameObjectList.clear();
	}
}