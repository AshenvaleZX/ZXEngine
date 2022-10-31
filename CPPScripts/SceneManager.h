#pragma once
#include "pubh.h"
#include "Scene.h"

namespace ZXEngine
{
	class SceneManager
	{
	public:
		SceneManager() {};
		~SceneManager() {};

		static void Create();
		static SceneManager* GetInstance();

		Scene* GetCurScene();

	private:
		static SceneManager* mInstance;

		map<string, Scene> scenes;
		Scene* curScene = nullptr;
	};
}