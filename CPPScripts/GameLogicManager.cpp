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
		for (auto gameLogic : allGameLogic)
		{
			gameLogic->Update();
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
		auto l = std::find(allGameLogic.begin(), allGameLogic.end(), gameLogic);
		allGameLogic.erase(l);
	}
}