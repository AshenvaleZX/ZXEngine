#include "Game.h"
#include "EventManager.h"
#include "RenderEngine.h"
#include "InputManager.h"
#include "Time.h"
#include "SceneManager.h"
#include "LuaManager.h"
#include "GameLogicManager.h"
#include "ProjectSetting.h"
#include "Editor/EditorGUIManager.h"
#include "Editor/EditorDataManager.h"

namespace ZXEngine
{
	void Game::Play()
	{
		ProjectSetting::InitSetting("../../ExampleProject");
		EventManager::Create();
		RenderEngine::Create();
		InputManager::Create();
		LuaManager::Create();
		GameLogicManager::Create();
		SceneManager::Create();
#ifdef ZX_EDITOR
		EditorGUIManager::Create();
		EditorDataManager::Create();
#endif

		while (!RenderEngine::GetInstance()->WindowShouldClose())
		{
			// Âß¼­
			Update();

			// äÖÈ¾
			Render();

#ifdef ZX_DEBUG
			// µ÷ÊÔ
			Debug::Update();
#endif
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

#ifdef ZX_EDITOR
		EditorGUIManager::GetInstance()->BeginEditorRender();
		EditorGUIManager::GetInstance()->EditorRender();
		EditorGUIManager::GetInstance()->EndEditorRender();
#endif

		RenderEngine::GetInstance()->EndRender();
	}
}