#include "Game.h"
#include "RenderEngine.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"

namespace ZXEngine
{
	void Game::Play()
	{
		RenderAPI::Creat();
		RenderQueueManager::Creat();
		SceneManager::Create();
		RenderEngine::Create();
		RenderEngine::InitWindow(1280, 720);
		Resources::SetAssetsPath("../../ExampleProject/Assets/");

		while (!RenderEngine::WindowShouldClose())
		{
			Render();
		}
	}

	void Game::Render()
	{
		SceneManager::GetInstance()->GetCurScene()->Render();

		RenderEngine::SwapBufferAndPollPollEvents();
	}
}