#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	GameObject::GameObject(PrefabStruct* prefab)
	{
		for (auto component : prefab->components)
		{
			if (component["Type"] == "Transform")
			{
				Transform* transform = AddComponent<Transform>("Transform");
			}
			else if (component["Type"] == "MeshRenderer")
			{
				MeshRenderer* meshRenderer = AddComponent<MeshRenderer>("MeshRenderer");

				string p = to_string(component["Material"]);
				// 这个json字符串取出来前后会有双引号，需要去掉再用
				p = p.substr(1, p.length() - 2);
				MaterialStruct* matStruct = Resources::LoadMaterial(p.c_str());
				meshRenderer->matetrial = new Material(matStruct);
			}
		}
	}

	void GameObject::AddComponent(string type, Component* component)
	{
		components.insert(pair<string, Component*>(type, component));
	}
}