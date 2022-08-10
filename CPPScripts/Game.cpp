#include "Game.h"

namespace ZXEngine
{
	Game* Game::mInstance;
	SceneManager* SceneManager::mInstance;

	void Game::Play()
	{
		SceneManager::mInstance = new SceneManager();
		cout << "ZXTest" << endl;
		Render();
	}

	void Game::Render()
	{
		SceneManager::mInstance->GetCurScene()->Render();
	}
}