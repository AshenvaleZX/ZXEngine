#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ZCamera.h"
#include "Light.h"
#include "GameLogic.h"
#include "UITextRenderer.h"

namespace ZXEngine
{
	GameObject::GameObject(PrefabStruct* prefab)
	{
		layer = prefab->layer;

		for (auto component : prefab->components)
		{
			if (component["Type"] == "Transform")
			{
				Transform* transform = AddComponent<Transform>("Transform");

				transform->position = vec3(component["Position"][0], component["Position"][1], component["Position"][2]);
				transform->rotation.SetEulerAngles(component["Rotation"][0], component["Rotation"][1], component["Rotation"][2]);
				transform->scale = vec3(component["Scale"][0], component["Scale"][1], component["Scale"][2]);
			}
			else if (component["Type"] == "MeshRenderer")
			{
				MeshRenderer* meshRenderer = AddComponent<MeshRenderer>("MeshRenderer");
				string p = "";

				meshRenderer->castShadow = component["CastShadow"];
				meshRenderer->receiveShadow = component["ReceiveShadow"];

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
				camera->nearClipDis = component["NearClipDis"];
				camera->farClipDis = component["FarClipDis"];
			}
			else if (component["Type"] == "Light")
			{
				Light* light = AddComponent<Light>("Light");

				light->color = vec3(component["Color"][0], component["Color"][1], component["Color"][2]);
				light->intensity = component["Intensity"];
				light->type = component["LightType"];
			}
			else if (component["Type"] == "GameLogic")
			{
				GameLogic* gameLogic = AddComponent<GameLogic>("GameLogic");
				gameLogic->luaName = Resources::JsonStrToString(component["Lua"]);
			}
			else if (component["Type"] == "UITextRenderer")
			{
				UITextRenderer* uiTextRenderer = AddComponent<UITextRenderer>("UITextRenderer");
				uiTextRenderer->text = component["Text"];
				uiTextRenderer->color = vec4(component["Color"][0], component["Color"][1], component["Color"][2], component["Color"][3]);
			}
		}
	}

	void GameObject::AddComponent(string type, Component* component)
	{
		component->gameObject = this;
		components.insert(pair<string, Component*>(type, component));
	}
}