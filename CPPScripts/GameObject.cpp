#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ZCamera.h"
#include "Light.h"

namespace ZXEngine
{
	GameObject::GameObject(PrefabStruct* prefab)
	{
		for (auto component : prefab->components)
		{
			if (component["Type"] == "Transform")
			{
				Transform* transform = AddComponent<Transform>("Transform");

				transform->position = vec3(component["Position"][0], component["Position"][1], component["Position"][2]);
				transform->rotation = vec3(component["Rotation"][0], component["Rotation"][1], component["Rotation"][2]);
			}
			else if (component["Type"] == "MeshRenderer")
			{
				MeshRenderer* meshRenderer = AddComponent<MeshRenderer>("MeshRenderer");
				string p = "";

				// ²ÄÖÊ
				if (component["Material"].is_null())
				{
					Debug::LogWarning("No material !");
				}
				else
				{
					p = Resources::JsonStrToString(component["Material"]);
					MaterialStruct* matStruct = Resources::LoadMaterial(p.c_str());
					meshRenderer->matetrial = new Material(matStruct);
				}

				// Mesh
				if (component["Mesh"].is_null())
				{
					Debug::LogWarning("No meshs !");
				}
				else
				{
					p = Resources::JsonStrToString(component["Mesh"]);
					p = Resources::GetAssetFullPath(p.c_str());
					meshRenderer->LoadModel(p);
				}
			}
			else if (component["Type"] == "Camera")
			{
				Camera* camera = AddComponent<Camera>("Camera");
			}
			else if (component["Type"] == "Light")
			{
				Light* light = AddComponent<Light>("Light");

				light->color = vec3(component["Color"][0], component["Color"][1], component["Color"][2]);
				light->intensity = component["Intensity"];
				light->type = component["LightType"];
			}
		}
	}

	void GameObject::AddComponent(string type, Component* component)
	{
		component->gameObject = this;
		components.insert(pair<string, Component*>(type, component));
	}
}