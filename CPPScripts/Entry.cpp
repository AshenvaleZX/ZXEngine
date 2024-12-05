/*  ______   ___  ___   ______   ___   __   ________   ______   ___   __   ______
|  |___   |  \  \/  /  |  ____| |   \ |  | |   _____| |__  __| |   \ |  | |  ____|
|     /  /    \    /   |  |___  | \  \|  | |  | ____    |  |   | \  \|  | |  |___
|    /  /     /    \   |   ___| |  \  \  | |  ||__  |   |  |   |  \  \  | |   ___|
|   /  /__   /  /\  \  |  |___  |  |\  \ | |  |__|  |  _|  |_  |  |\  \ | |  |___
|  |______| |__/  \__| |______| |__| \___| |________| |______| |__| \___| |______|
|
|  C++ game engine project, for learning, practice and experiment.
|  Repository: https://github.com/AshenvaleZX/ZXEngine
|  License: GPL-3.0
*/

#include "Game.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
	string path = ZXEngine::Utils::GetCurrentExecutableFilePath() + "/../../../ExampleProject";
	if (argc > 1) path = argv[1];

	if (ZXEngine::Game::Launch(path))
	{
		ZXEngine::Game::Run();
		ZXEngine::Game::Shutdown();
	}

	return 0;
}