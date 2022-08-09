#pragma once
#include "pubh.h"
#include "Scene.h"

namespace ZXEngine
{
	class SceneManager
	{
	public:
		static SceneManager* mInstance;

		SceneManager();
		~SceneManager();

		Scene* GetCurScene();

	private:
		map<string, Scene> scenes;
		Scene* curScene;
	};
}