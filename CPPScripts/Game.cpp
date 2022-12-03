#include "Game.h"
#include "EventManager.h"
#include "RenderEngine.h"
#include "InputManager.h"
#include "Time.h"
#include "SceneManager.h"
#include "Resources.h"
#include "LuaManager.h"
#include "GameLogicManager.h"

namespace ZXEngine
{
	void Game::Play()
	{
		Resources::SetAssetsPath("../../ExampleProject/Assets/");
		EventManager::Create();
		SceneManager::Create();
		RenderEngine::Create();
		InputManager::Create();
		LuaManager::Create();
		GameLogicManager::Create();
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

		GameLogicManager::GetInstance()->Update();
	}

	void Game::Render()
	{
		RenderEngine::GetInstance()->BeginRender();

		SceneManager::GetInstance()->GetCurScene()->Render();

		RenderEngine::GetInstance()->EndRender();
	}
}