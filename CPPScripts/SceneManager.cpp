#include "SceneManager.h"
#include "Resources.h"

namespace ZXEngine
{
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