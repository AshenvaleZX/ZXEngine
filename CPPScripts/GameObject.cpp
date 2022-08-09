#include "GameObject.h"

namespace ZXEngine
{
	// ����ʱ����ʵ��һ�£������ٿ���ôд������һ��
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