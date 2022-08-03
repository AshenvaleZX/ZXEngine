#include <iostream>
#include "Game.h"

namespace ZXEngine
{
	Game* Game::mInstance;

	void Game::Play()
	{
		std::cout << "ZXTest";
	}
}