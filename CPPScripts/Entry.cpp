#include "Game.h"

ZXEngine::Game* ZXEngine::Game::mInstance;

int main(int argc, char* argv[])
{
	string path = "../../ExampleProject";
	if (argc > 1) path = argv[1];

	ZXEngine::Game::mInstance = new ZXEngine::Game();
	ZXEngine::Game::mInstance->Launch(path);
	return 0;
}