#include "SceneManager.h"

namespace ZXEngine
{
	Scene* SceneManager::GetCurScene()
	{
		if (curScene == nullptr)
			curScene = new Scene();

		return curScene;
	}
}