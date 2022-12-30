#include "GameObject.h"

namespace ZXEngine
{
	GameObject::GameObject(PrefabStruct* prefab)
	{
		name = prefab->name;
		layer = prefab->layer;

		for (auto component : prefab->components)
		{
			if (component["Type"] == "Transform")
			{
				Transform* transform = AddComponent<Transform>();

				transform->position = Vector3(component["Position"][0], component["Position"][1], component["Position"][2]);
				transform->rotation.SetEulerAngles(component["Rotation"][0], component["Rotation"][1], component["Rotation"][2]);
				transform->scale = Vector3(component["Scale"][0], component["Scale"][1], component["Scale"][2]);
			}
			else if (component["Type"] == "MeshRenderer")
			{
				MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
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
					MaterialStruct* matStruct = Resources::LoadMaterial(p);
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
					meshRenderer->modelName = Resources::GetAssetName(p);
					p = Resources::GetAssetFullPath(p);
					meshRenderer->LoadModel(p);
				}
			}
			else if (component["Type"] == "Camera")
			{
				Camera* camera = AddComponent<Camera>();
				camera->Fov = component["FOV"];
				camera->nearClipDis = component["NearClipDis"];
				camera->farClipDis = component["FarClipDis"];
				if (!component["Aspect"].is_null())
					camera->aspect = component["Aspect"];
			}
			else if (component["Type"] == "Light")
			{
				Light* light = AddComponent<Light>();

				light->color = Vector3(component["Color"][0], component["Color"][1], component["Color"][2]);
				light->intensity = component["Intensity"];
				light->type = component["LightType"];
			}
			else if (component["Type"] == "GameLogic")
			{
				GameLogic* gameLogic = AddComponent<GameLogic>();
				gameLogic->luaName = Resources::JsonStrToString(component["Lua"]);
			}
			else if (component["Type"] == "UITextRenderer")
			{
				UITextRenderer* uiTextRenderer = AddComponent<UITextRenderer>();
				uiTextRenderer->text = component["Text"];
				uiTextRenderer->color = Vector4(component["Color"][0], component["Color"][1], component["Color"][2], component["Color"][3]);
			}
			else if (component["Type"] == "UITextureRenderer")
			{
				UITextureRenderer* uiTextureRenderer = AddComponent<UITextureRenderer>();
				string p = Resources::JsonStrToString(component["TexturePath"]);
				uiTextureRenderer->SetTexture(Resources::GetAssetFullPath(p.c_str()).c_str());
			}
			else if (component["Type"] == "ParticleSystem")
			{
				ParticleSystem* particleSystem = AddComponent<ParticleSystem>();
				string p = Resources::JsonStrToString(component["TexturePath"]);
				particleSystem->SetTexture(Resources::GetAssetFullPath(p.c_str()).c_str());
				particleSystem->particleNum = component["ParticleNum"];
				particleSystem->lifeTime = component["LifeTime"];
				particleSystem->velocity = Vector3(component["Velocity"][0], component["Velocity"][1], component["Velocity"][2]);
				particleSystem->offset = Vector3(component["StartOffset"][0], component["StartOffset"][1], component["StartOffset"][2]);
				particleSystem->GenerateParticles();
			}
		}

		for (auto subPrefab : prefab->children)
		{
			auto subGameObject = new GameObject(subPrefab);
			subGameObject->parent = this;
			children.push_back(subGameObject);
		}
	}

	void GameObject::AddComponent(ComponentType type, Component* component)
	{
		component->gameObject = this;
		components.insert(pair<ComponentType, Component*>(type, component));
	}
}