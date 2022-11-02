#include "Game.h"
#include "EventManager.h"
#include "RenderEngine.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"
#include "InputManager.h"
#include "Time.h"

namespace ZXEngine
{
	void Game::Play()
	{
		EventManager::Create();
		RenderAPI::Creat();
		RenderQueueManager::Creat();
		SceneManager::Create();
		RenderEngine::Create();
		RenderEngine::GetInstance()->InitWindow(1280, 720);
		InputManager::Create();
		Resources::SetAssetsPath("../../ExampleProject/Assets/");

		while (!RenderEngine::GetInstance()->WindowShouldClose())
		{
			// Âß¼­
			Update();

			// äÖÈ¾
			Render();
		}
	}

	void Game::Update()
	{
		Time::Update();

		InputManager::GetInstance()->Update();
	}

	void Game::Render()
	{
		RenderEngine::GetInstance()->BeginRender();

		SceneManager::GetInstance()->GetCurScene()->Render();

		RenderEngine::GetInstance()->SwapBufferAndPollPollEvents();
	}
}