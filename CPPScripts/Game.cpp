#include "Game.h"
#include "RenderEngine.h"
#include "RenderAPI.h"

namespace ZXEngine
{
	SceneManager* SceneManager::mInstance;
	RenderEngine* RenderEngine::mInstance;

	void Game::Play()
	{
		SceneManager::mInstance = new SceneManager();
		RenderEngine::mInstance = new RenderEngine();
		RenderEngine::mInstance->SetRenderAPI(RenderAPI::Creat());
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