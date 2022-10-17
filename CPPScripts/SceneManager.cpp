#include "SceneManager.h"

namespace ZXEngine
{
	SceneManager* SceneManager::GetInstance()
	{
		return mInstance;
	}

	Scene* SceneManager::GetCurScene()
	{
		if (curScene == nullptr)
			curScene = new Scene();

		return curScene;
	}
}