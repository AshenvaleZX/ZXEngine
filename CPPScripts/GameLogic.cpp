#include "GameLogic.h"
#include "GameLogicManager.h"

namespace ZXEngine
{
	GameLogic::GameLogic()
	{
		GameLogicManager::GetInstance()->AddGameLogic(this);
	}

	GameLogic::~GameLogic()
	{
		GameLogicManager::GetInstance()->RemoveGameLogic(this);
	}

	void GameLogic::Update()
	{
		
	}
}