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
#include "Component/Animator.h"
#include "Audio/AudioEngine.h"

#ifdef ZX_EDITOR
#include "Editor/EditorGUIManager.h"
#include "Editor/EditorDataManager.h"
#include "Editor/EditorInputManager.h"
#endif

namespace ZXEngine
{
	void Game::Launch(const string& path)
	{
#ifdef ZX_EDITOR
		EditorDataManager::Create();
#endif

		if (!ProjectSetting::InitSetting(path))
		{
			std::cerr << "Invalid project path: " << path << std::endl;
			return;
		}
		else
		{
			std::cout << "ZXEngine launch project: " << path << std::endl;
		}

		EventManager::Create();
		AudioEngine::Create();
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

#ifdef ZX_EDITOR
		EditorInputManager::GetInstance()->Update();
#endif

#ifdef ZX_EDITOR
		if (EditorDataManager::isGameStart && !EditorDataManager::isGamePause)
		{
			Animator::Update();
			SceneManager::GetInstance()->GetCurScene()->UpdatePhysics();
			GameLogicManager::GetInstance()->Update();
			ParticleSystemManager::GetInstance()->Update();
		}
#else
		Animator::Update();
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