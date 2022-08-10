#include "Game.h"

ZXEngine::Game* ZXEngine::Game::mInstance;

int main()
{
	ZXEngine::Game::mInstance = new ZXEngine::Game();
	ZXEngine::Game::mInstance->Play();
	return 0;
}