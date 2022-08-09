#include "GameObject.h"

namespace ZXEngine
{
	// 先临时这样实现一下，后面再看怎么写得优雅一点
	Component* GameObject::GetComponent(string type)
	{
		if (type == "Transform")
			return transform;
		else if (type == "MeshRenderer")
			return meshRenderer;
		else
			return nullptr;
	}

}