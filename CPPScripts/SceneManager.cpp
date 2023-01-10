#include "SceneManager.h"
#include "Resources.h"
#include "ProjectSetting.h"

namespace ZXEngine
{
	SceneManager* SceneManager::mInstance = nullptr;

	void SceneManager::Create()
	{
		mInstance = new SceneManager();
		mInstance->LoadScene(ProjectSetting::defaultScene);
	}

	SceneManager* SceneManager::GetInstance()
	{
		return mInstance;
	}

	Scene* SceneManager::GetScene(string name)
	{
		auto sceneInfo = GetSceneInfo(name);
		if (sceneInfo == nullptr)
		{
			Debug::LogError("Get a nonexistent scene: " + name);
			return nullptr;
		}
		else
		{
			return sceneInfo->scene;
		}
	}

	void SceneManager::LoadScene(string path, bool switchNow)
	{
		string name = Resources::GetAssetName(path);

		if (GetScene(name) == nullptr)
		{
			SceneInfo* info = new SceneInfo();
			info->path = path;
			info->scene = new Scene(Resources::LoadScene(path));
			scenes.insert(pair<string, SceneInfo*>(name, info));

			if (switchNow)
				curScene = info;
		}
		else
		{
			Debug::LogError("Load scene failed: " + path);
		}
	}

	void SceneManager::SwitchScene(string name)
	{
		auto scene = GetSceneInfo(name);
		if (scene == nullptr)
			Debug::LogError("Switch to invalid scene: " + name);
		else
			curScene = scene;
	}

	void SceneManager::DeleteScene(string name)
	{
		map<string, SceneInfo*>::iterator iter = scenes.find(name);
		if (iter != scenes.end())
		{
			delete iter->second;
			scenes.erase(iter);
		}
		else
		{
			Debug::LogWarning("Try to delete a nonexistent scene: " + name);
		}
	}

	void SceneManager::ReloadScene()
	{
		delete curScene->scene;
		curScene->scene = new Scene(Resources::LoadScene(curScene->path));
	}

	Scene* SceneManager::GetCurScene()
	{
		return curScene->scene;
	}

	SceneInfo* SceneManager::GetSceneInfo(string name)
	{
		map<string, SceneInfo*>::iterator iter = scenes.find(name);
		if (iter != scenes.end())
			return iter->second;
		else
			return nullptr;
	}
}