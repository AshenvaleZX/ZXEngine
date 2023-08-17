#include "Game.h"
#include "EventManager.h"
#include "RenderEngine.h"
#include "Input/InputManager.h"
#include "Time.h"
#include "SceneManager.h"
#include "LuaManager.h"
#include "GameLogicManager.h"
#include "ProjectSetting.h"
#include "ParticleSystemManager.h"
#ifdef ZX_EDITOR
#include "Editor/EditorGUIManager.h"
#include "Editor/EditorDataManager.h"
#include "Editor/EditorInputManager.h"
#endif

namespace ZXEngine
{
	void Game::Play()
	{
#ifdef ZX_EDITOR
		EditorDataManager::Create();
#endif
		ProjectSetting::InitSetting("../../ExampleProject");
		EventManager::Create();
		RenderEngine::Create();
		InputManager::Create();
		LuaManager::Create();
		GameLogicManager::Create();
		SceneManager::Create();
#ifdef ZX_EDITOR
		EditorGUIManager::Create();
		EditorInputManager::Create();
#endif

		while (!RenderEngine::GetInstance()->WindowShouldClose())
		{
			// �߼�
			Update();

			// ��Ⱦ
			Render();

#ifdef ZX_DEBUG
			// ����
			Debug::Update();
#endif
		}
	}

	void Game::Update()
	{
		Time::Update();

		InputManager::GetInstance()->Update();

#ifdef ZX_EDITOR
		EditorInputManager::GetInstance()->Update();
#endif

#ifdef ZX_EDITOR
		if (EditorDataManager::isGameStart && !EditorDataManager::isGamePause)
		{
			SceneManager::GetInstance()->GetCurScene()->UpdatePhysics();
			GameLogicManager::GetInstance()->Update();
			ParticleSystemManager::GetInstance()->Update();
		}
#else
		SceneManager::GetInstance()->GetCurScene()->UpdatePhysics();
		GameLogicManager::GetInstance()->Update();
		ParticleSystemManager::GetInstance()->Update();
#endif
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