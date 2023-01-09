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
				ParseTransform(component);
			else if (component["Type"] == "MeshRenderer")
				ParseMeshRenderer(component);
			else if (component["Type"] == "Camera")
				ParseCamera(component);
			else if (component["Type"] == "Light")
				ParseLight(component);
			else if (component["Type"] == "GameLogic")
				ParseGameLogic(component);
			else if (component["Type"] == "UITextRenderer")
				ParseUITextRenderer(component);
			else if (component["Type"] == "UITextureRenderer")
				ParseUITextureRenderer(component);
			else if (component["Type"] == "ParticleSystem")
				ParseParticleSystem(component);
			else
				Debug::LogError("Try parse undefined component type: " + component["Type"]);
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

	void GameObject::ParseTransform(json data)
	{
		Transform* transform = AddComponent<Transform>();

		transform->SetLocalPosition(Vector3(data["Position"][0], data["Position"][1], data["Position"][2]));
		transform->SetLocalEulerAngles(data["Rotation"][0], data["Rotation"][1], data["Rotation"][2]);
		transform->SetLocalScale(Vector3(data["Scale"][0], data["Scale"][1], data["Scale"][2]));
	}

	void GameObject::ParseMeshRenderer(json data)
	{
		MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
		string p = "";

		meshRenderer->castShadow = data["CastShadow"];
		meshRenderer->receiveShadow = data["ReceiveShadow"];

		// ²ÄÖÊ
		if (data["Material"].is_null())
		{
			Debug::LogWarning("No material !");
		}
		else
		{
			p = Resources::JsonStrToString(data["Material"]);
			MaterialStruct* matStruct = Resources::LoadMaterial(p);
			meshRenderer->matetrial = new Material(matStruct);
		}

		// Mesh
		if (data["Mesh"].is_null())
		{
			Debug::LogWarning("No meshs !");
		}
		else
		{
			p = Resources::JsonStrToString(data["Mesh"]);
			meshRenderer->modelName = Resources::GetAssetName(p);
			p = Resources::GetAssetFullPath(p);
			meshRenderer->LoadModel(p);
		}
	}

	void GameObject::ParseCamera(json data)
	{
		Camera* camera = AddComponent<Camera>();

		camera->Fov = data["FOV"];
		camera->nearClipDis = data["NearClipDis"];
		camera->farClipDis = data["FarClipDis"];
		if (!data["Aspect"].is_null())
			camera->aspect = data["Aspect"];
	}

	void GameObject::ParseLight(json data)
	{
		Light* light = AddComponent<Light>();

		light->color = Vector3(data["Color"][0], data["Color"][1], data["Color"][2]);
		light->intensity = data["Intensity"];
		light->type = data["LightType"];
	}

	void GameObject::ParseGameLogic(json data)
	{
		GameLogic* gameLogic = AddComponent<GameLogic>();

		string p = Resources::JsonStrToString(data["Lua"]);
		gameLogic->luaName = Resources::GetAssetName(p);
		gameLogic->luaFullPath = Resources::GetAssetFullPath(p);
	}

	void GameObject::ParseUITextRenderer(json data)
	{
		UITextRenderer* uiTextRenderer = AddComponent<UITextRenderer>();

		uiTextRenderer->text = data["Text"];
		uiTextRenderer->color = Vector4(data["Color"][0], data["Color"][1], data["Color"][2], data["Color"][3]);
	}

	void GameObject::ParseUITextureRenderer(json data)
	{
		UITextureRenderer* uiTextureRenderer = AddComponent<UITextureRenderer>();

		string p = Resources::JsonStrToString(data["TexturePath"]);
		uiTextureRenderer->SetTexture(Resources::GetAssetFullPath(p).c_str());
	}

	void GameObject::ParseParticleSystem(json data)
	{
		ParticleSystem* particleSystem = AddComponent<ParticleSystem>();

		string p = Resources::JsonStrToString(data["TexturePath"]);
		particleSystem->SetTexture(Resources::GetAssetFullPath(p).c_str());
		particleSystem->particleNum = data["ParticleNum"];
		particleSystem->lifeTime = data["LifeTime"];
		particleSystem->velocity = Vector3(data["Velocity"][0], data["Velocity"][1], data["Velocity"][2]);
		particleSystem->offset = Vector3(data["StartOffset"][0], data["StartOffset"][1], data["StartOffset"][2]);
		particleSystem->GenerateParticles();
	}
}