#include "SceneManager.h"
#include "Resources.h"

namespace ZXEngine
{
	SceneManager* SceneManager::mInstance = nullptr;

	void SceneManager::Create()
	{
		mInstance = new SceneManager();
	}

	SceneManager* SceneManager::GetInstance()
	{
		return mInstance;
	}

	Scene* SceneManager::GetCurScene()
	{
		if (curScene == nullptr)
			curScene = new Scene(Resources::LoadScene("Scenes/MyWorld.zxscene"));

		return curScene;
	}
}