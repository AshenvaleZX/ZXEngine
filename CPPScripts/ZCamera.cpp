#include "ZCamera.h"
#include "MeshRenderer.h"
#include "Scene.h"

namespace ZXEngine
{
	void Camera::Render(list<GameObject*> gameObjects)
	{
		for (auto go : gameObjects)
		{
			MeshRenderer* meshRenderer = go->GetComponent<MeshRenderer>("MeshRenderer");
		}

	}
}