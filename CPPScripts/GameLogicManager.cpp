#include "GameLogicManager.h"
#include "Component/GameLogic.h"

namespace ZXEngine
{
	GameLogicManager* GameLogicManager::mInstance = nullptr;

	void GameLogicManager::Create()
	{
		mInstance = new GameLogicManager();
	}

	GameLogicManager* GameLogicManager::GetInstance()
	{
		return mInstance;
	}

	void GameLogicManager::Update()
	{
		// 逆序遍历，因为遍历过程中可能会删除元素
		for (int i = static_cast<int>(allGameLogic.size()) - 1; i >= 0; i--)
		{
			allGameLogic[i]->Update();
		}
	}

	void GameLogicManager::FixedUpdate()
	{
		for (auto gameLogic : allGameLogic)
		{
			gameLogic->FixedUpdate();
		}
	}

	void GameLogicManager::AddGameLogic(GameLogic* gameLogic)
	{
		allGameLogic.push_back(gameLogic);
	}

	void GameLogicManager::RemoveGameLogic(GameLogic* gameLogic)
	{
		auto iter = std::find(allGameLogic.begin(), allGameLogic.end(), gameLogic);
		
		if (iter != allGameLogic.end())
			allGameLogic.erase(iter);
	}
}