#include "Game.h"
#include "EventManager.h"
#include "RenderEngine.h"
#include "Input/InputManager.h"
#include "Time.h"
#include "Utils.h"
#include "SceneManager.h"
#include "LuaManager.h"
#include "GameLogicManager.h"
#include "ProjectSetting.h"
#include "Component/Animator.h"
#include "Audio/AudioEngine.h"
#include "Resources.h"
#include "Concurrent/JobSystem.h"
#include "ECS/ECS.h"

#ifdef ZX_EDITOR
#include "Editor/EditorGUIManager.h"
#include "Editor/EditorDataManager.h"
#include "Editor/EditorInputManager.h"
#endif

namespace ZXEngine
{
	bool Game::Launch(const string& path)
	{
#ifdef ZX_EDITOR
		EditorDataManager::Create();
#endif

		if (!ProjectSetting::InitSetting(path))
		{
			std::cerr << "Invalid project path: " << Utils::RelativePathToAbsolutePath(path) << std::endl;
			return false;
		}
		else
		{
			std::cout << "ZXEngine launch project: " << path << std::endl;
		}

		ECS::World::Create();
		EventManager::Create();
		JobSystem::Create();
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

		return true;
	}

	void Game::Run()
	{
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

	void Game::Shutdown()
	{
		JobSystem::Destroy();
	}

	void Game::Update()
	{
		Time::Update();

		Resources::CheckAsyncLoad();

		InputManager::GetInstance()->Update();

#ifdef ZX_EDITOR
		EditorInputManager::GetInstance()->Update();
#endif

#ifdef ZX_EDITOR
		if (EditorDataManager::isGameStart && !EditorDataManager::isGamePause)
		{
			SceneManager::GetInstance()->GetCurScene()->Update();
		}
#else
		SceneManager::GetInstance()->GetCurScene()->Update();
#endif
	}

	void Game::Render()
	{
		RenderEngine::GetInstance()->BeginRender();

		SceneManager::GetInstance()->GetCurScene()->Render();

#ifdef ZX_EDITOR
		EditorGUIManager::GetInstance()->Render();
#endif

		RenderEngine::GetInstance()->EndRender();
	}
}