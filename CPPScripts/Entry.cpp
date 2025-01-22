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

#if defined(ZX_PLATFORM_DESKTOP)

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

#elif defined(ZX_PLATFORM_ANDROID)

#include "GlobalData.h"
void android_main(struct android_app* app)
{
	ZXEngine::GlobalData::app = app;
	
	if (ZXEngine::Game::Launch(""))
	{
		ZXEngine::Game::Run();
		ZXEngine::Game::Shutdown();
	}
	else
	{
		ZXEngine::Debug::LogError("Launch failed, exiting program.");
		std::terminate();
	}
}

#endif