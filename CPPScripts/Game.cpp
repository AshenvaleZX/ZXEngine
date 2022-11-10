#include "Game.h"
#include "EventManager.h"
#include "RenderEngine.h"
#include "RenderAPI.h"
#include "RenderQueueManager.h"
#include "InputManager.h"
#include "Time.h"
#include "SceneManager.h"
#include "Resources.h"
#include "RenderPassManager.h"

namespace ZXEngine
{
	void Game::Play()
	{
		EventManager::Create();
		RenderQueueManager::Creat();
		SceneManager::Create();
		RenderPassManager::Create();
		RenderEngine::Create();
		RenderEngine::GetInstance()->InitWindow(1280, 720);
		RenderAPI::Creat();
		RenderAPI::GetInstance()->InitRenderSetting();
		InputManager::Create();
		Resources::SetAssetsPath("../../ExampleProject/Assets/");
		RenderEngine::GetInstance()->InitSkyBox();
		SceneManager::GetInstance()->SetCurScene("Scenes/MyWorld.zxscene");

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

		RenderEngine::GetInstance()->EndRender();
	}
}