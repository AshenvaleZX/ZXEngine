#include "Game.h"

namespace ZXEngine
{
	Game* Game::mInstance;
	SceneManager* SceneManager::mInstance;

	void Game::Play()
	{
		cout << "ZXTest";
		Render();
	}

	void Game::Render()
	{
		SceneManager::mInstance->GetCurScene()->Render();
	}
}