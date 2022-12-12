#include "SceneManager.h"
#include "Resources.h"
#include "ProjectSetting.h"

namespace ZXEngine
{
	SceneManager* SceneManager::mInstance = nullptr;

	void SceneManager::Create()
	{
		mInstance = new SceneManager();
		mInstance->SetCurScene(ProjectSetting::defaultScene.c_str());
	}

	SceneManager* SceneManager::GetInstance()
	{
		return mInstance;
	}

	Scene* SceneManager::GetCurScene()
	{
		return curScene;
	}

	void SceneManager::SetCurScene(const char* path)
	{
		curScene = new Scene(Resources::LoadScene(path));
	}
}