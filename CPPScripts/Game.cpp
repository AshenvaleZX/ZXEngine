#include "Game.h"
#include "RenderEngine.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"
#include "InputManager.h"

namespace ZXEngine
{
	void Game::Play()
	{
		RenderAPI::Creat();
		RenderQueueManager::Creat();
		SceneManager::Create();
		RenderEngine::Create();
		RenderEngine::InitWindow(1280, 720);
		InputManager::Create();
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