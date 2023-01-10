#pragma once
#include "pubh.h"
#include "Scene.h"

namespace ZXEngine
{
	struct SceneInfo
	{
		string path;
		Scene* scene = nullptr;
		~SceneInfo() { delete scene; };
	};

	class SceneManager
	{
	public:
		static void Create();
		static SceneManager* GetInstance();

	private:
		static SceneManager* mInstance;

	public:
		SceneManager() {};
		~SceneManager() {};

		Scene* GetScene(string name);
		void LoadScene(string path, bool switchNow = true);
		void SwitchScene(string name);
		void DeleteScene(string name);
		void ReloadScene();
		Scene* GetCurScene();

	private:
		SceneInfo* curScene = nullptr;
		map<string, SceneInfo*> scenes;

		SceneInfo* GetSceneInfo(string name);
	};
}