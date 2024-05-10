#include "RenderQueueManager.h"
#include "GameObject.h"
#include "Component/MeshRenderer.h"
#include "Material.h"

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

	RenderQueueManager::RenderQueueManager()
	{
		renderQueues =
		{
			{ (int)RenderQueueType::Deferred,    new RenderQueue() },
			{ (int)RenderQueueType::Opaque,      new RenderQueue() },
			{ (int)RenderQueueType::Transparent, new RenderQueue() },
		};
	}

	RenderQueueManager::~RenderQueueManager()
	{
		for (auto& iter : renderQueues)
		{
			delete iter.second;
		}
	}

	RenderQueue* RenderQueueManager::GetRenderQueue(int queue)
	{
		auto iter = renderQueues.find(queue);
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
			if (gameObject->IsActive())
			{
				AddUIGameObject(gameObject);
			}
		}
		else
		{
			MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
			if (meshRenderer != nullptr && meshRenderer->IsActive())
			{
				AddRenderer(meshRenderer);
			}
		}

		for (auto subGameObject : gameObject->children)
			AddGameObject(subGameObject);
	}

	void RenderQueueManager::AddRenderer(MeshRenderer* meshRenderer)
	{
		int queue = meshRenderer->mMatetrial->GetRenderQueue();
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