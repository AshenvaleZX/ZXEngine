#include "ZCamera.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	void Camera::Render(list<GameObject*> gameObjects)
	{
		for (auto go : gameObjects)
		{
			auto meshRenderer = go->GetComponent("MeshRenderer");
		}

	}
}