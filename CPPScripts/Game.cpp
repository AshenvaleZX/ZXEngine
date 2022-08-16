#include "Game.h"
#include "RenderEngine.h"

namespace ZXEngine
{
	SceneManager* SceneManager::mInstance;

	void Game::Play()
	{
		SceneManager::mInstance = new SceneManager();
		RenderEngine::InitWindow(1280, 720);
		Debug::Log("ZXTest");

		while (!RenderEngine::WindowShouldClose())
		{
			Render();
		}
	}

	void Game::Render()
	{
		SceneManager::mInstance->GetCurScene()->Render();

		RenderEngine::SwapBufferAndPollPollEvents();
	}
}