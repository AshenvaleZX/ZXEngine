#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class GameLogic;
	class GameLogicManager
	{
	public:
		GameLogicManager() {};
		~GameLogicManager() {};

		static void Create();
		static GameLogicManager* GetInstance();

		void Update();
		void AddGameLogic(GameLogic* gameLogic);
		void RemoveGameLogic(GameLogic* gameLogic);

	private:
		static GameLogicManager* mInstance;
		list<GameLogic*> allGameLogic;

	};
}