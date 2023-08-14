#include "GameObject.h"

namespace ZXEngine
{
	GameObject::GameObject(PrefabStruct* prefab)
	{
		name = prefab->name;
		layer = prefab->layer;

		for (auto& component : prefab->components)
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
			else if (component["Type"] == "BoxCollider")
				ParseBoxCollider(component);
			else if (component["Type"] == "PlaneCollider")
				ParsePlaneCollider(component);
			else if (component["Type"] == "SphereCollider")
				ParseSphereCollider(component);
			else if (component["Type"] == "RigidBody")
				ParseRigidBody(component);
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

	GameObject::~GameObject()
	{
		for (auto& iter : components)
		{
			if (iter.first == ComponentType::Transform)
				delete static_cast<Transform*>(iter.second);
			else if (iter.first == ComponentType::MeshRenderer)
				delete static_cast<MeshRenderer*>(iter.second);
			else if (iter.first == ComponentType::Camera)
				delete static_cast<Camera*>(iter.second);
			else if (iter.first == ComponentType::Light)
				delete static_cast<Light*>(iter.second);
			else if (iter.first == ComponentType::GameLogic)
				delete static_cast<GameLogic*>(iter.second);
			else if (iter.first == ComponentType::UITextRenderer)
				delete static_cast<UITextRenderer*>(iter.second);
			else if (iter.first == ComponentType::UITextureRenderer)
				delete static_cast<UITextureRenderer*>(iter.second);
			else if (iter.first == ComponentType::ParticleSystem)
				delete static_cast<ParticleSystem*>(iter.second);
		}

		for (auto child : children)
			delete child;
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
		if (!data["Geometry"].is_null())
		{
			meshRenderer->GenerateGeometry(data["Geometry"]);
		}
		else if (!data["Mesh"].is_null())
		{
			p = Resources::JsonStrToString(data["Mesh"]);
			meshRenderer->modelName = Resources::GetAssetName(p);
			p = Resources::GetAssetFullPath(p);
			meshRenderer->LoadModel(p);
		}
		else
		{
			Debug::LogWarning("No meshs !");
		}
	}

	void GameObject::ParseCamera(json data)
	{
		Camera* camera = AddComponent<Camera>();

		camera->Fov = data["FOV"];
		camera->nearClipDis = data["NearClipDis"];
		camera->farClipDis = data["FarClipDis"];
		camera->enableAfterEffects = data["EnableAfterEffects"];
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

		uiTextRenderer->size = data["Size"];
		uiTextRenderer->SetContent(data["Text"]);
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

	void GameObject::ParseBoxCollider(json data)
	{
		BoxCollider* boxCollider = AddComponent<BoxCollider>();

		boxCollider->mFriction = data["Friction"];
		boxCollider->mBounciness = data["Bounciness"];
		boxCollider->mFrictionCombine = data["FrictionCombine"];
		boxCollider->mBounceCombine = data["BounceCombine"];

		boxCollider->mCollider->mHalfSize = Vector3(data["Size"][0] / 2.0f, data["Size"][1] / 2.0f, data["Size"][2] / 2.0f);
	}

	void GameObject::ParsePlaneCollider(json data)
	{
		PlaneCollider* planeCollider = AddComponent<PlaneCollider>();

		planeCollider->mFriction = data["Friction"];
		planeCollider->mBounciness = data["Bounciness"];
		planeCollider->mFrictionCombine = data["FrictionCombine"];
		planeCollider->mBounceCombine = data["BounceCombine"];

		planeCollider->mCollider->mNormal = Vector3(data["Normal"][0], data["Normal"][1], data["Normal"][2]);
		planeCollider->mCollider->mDistance = data["Distance"];
	}

	void GameObject::ParseSphereCollider(json data)
	{
		SphereCollider* sphereCollider = AddComponent<SphereCollider>();

		sphereCollider->mFriction = data["Friction"];
		sphereCollider->mBounciness = data["Bounciness"];
		sphereCollider->mFrictionCombine = data["FrictionCombine"];
		sphereCollider->mBounceCombine = data["BounceCombine"];

		sphereCollider->mCollider->mRadius = data["Radius"];
	}

	void GameObject::ParseRigidBody(json data)
	{
		ZRigidBody* rigidBody = AddComponent<ZRigidBody>();

		rigidBody->mUseGravity = data["UseGravity"];
		rigidBody->mRigidBody->SetMass(data["Mass"]);

		float damping = data["Damping"];
		rigidBody->mRigidBody->SetLinearDamping(1.0f - damping);

		float angularDamping = data["AngularDamping"];
		rigidBody->mRigidBody->SetAngularDamping(1.0f - angularDamping);
	}
}