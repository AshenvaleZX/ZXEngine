#include "GameObject.h"
#include "Material.h"
#include "ModelUtil.h"
#include "SceneManager.h"
#include "PhysZ/Force/FGGravity.h"

namespace ZXEngine
{
	GameObject* GameObject::Find(const string& path)
	{
		auto paths = Utils::StringSplit(path, '/');
		int recursion = 0;
		return Find(SceneManager::GetInstance()->GetCurScene()->gameObjects, paths, recursion);
	}

	GameObject* GameObject::Find(const vector<GameObject*>& gameObjects, const vector<string> paths, int& recursion)
	{
		if (recursion == paths.size())
			return nullptr;

		for (auto gameObject : gameObjects)
		{
			if (gameObject->name == paths[recursion])
			{
				recursion++;
				if (recursion == paths.size())
					return gameObject;
				else
					return Find(gameObject->children, paths, recursion);
			}
		}

		return nullptr;
	}

	GameObject* GameObject::FindChildren(const string& path)
	{
		auto paths = Utils::StringSplit(path, '/');
		int recursion = 0;
		return GameObject::Find(children, paths, recursion);
	}

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
			else if (iter.first == ComponentType::BoxCollider)
				delete static_cast<BoxCollider*>(iter.second);
			else if (iter.first == ComponentType::PlaneCollider)
				delete static_cast<PlaneCollider*>(iter.second);
			else if (iter.first == ComponentType::SphereCollider)
				delete static_cast<SphereCollider*>(iter.second);
			else if (iter.first == ComponentType::RigidBody)
				delete static_cast<ZRigidBody*>(iter.second);
			else if (iter.first == ComponentType::Animator)
				delete static_cast<Animator*>(iter.second);
			else
				Debug::LogError("Try delete undefined component type: %s", static_cast<int>(iter.first));
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

		meshRenderer->mCastShadow = data["CastShadow"];
		meshRenderer->mReceiveShadow = data["ReceiveShadow"];

		// 材质
		if (data["Material"].is_null())
		{
			Debug::LogWarning("No material !");
		}
		else
		{
			p = Resources::JsonStrToString(data["Material"]);
			MaterialStruct* matStruct = Resources::LoadMaterial(p);
			meshRenderer->mMatetrial = new Material(matStruct);
		}

		// Mesh
		if (!data["Geometry"].is_null())
		{
			GeometryType type = data["Geometry"];
			meshRenderer->GenerateModel(type);
		}
		else if (!data["Mesh"].is_null())
		{
			p = Resources::JsonStrToString(data["Mesh"]);
			meshRenderer->mModelName = Resources::GetAssetName(p);
			p = Resources::GetAssetFullPath(p);

			const ModelData& modelData = ModelUtil::LoadModel(p);
			meshRenderer->SetMeshes(modelData.pMeshes);

			if (modelData.pAnimationController)
			{
				Animator* animator = AddComponent<Animator>();
				animator->mAvatarName = meshRenderer->mModelName + "Avatar";
				animator->mRootBoneNode = modelData.pRootBoneNode;
				animator->mAnimationController = modelData.pAnimationController;
				// 为了方便，这里直接让MeshRenderer和Animator互相引用
				meshRenderer->mAnimator = animator;
				animator->mMeshRenderer = meshRenderer;
			}
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

		mColliderType = PhysZ::ColliderType::Box;

		boxCollider->mFriction = data["Friction"];
		boxCollider->mBounciness = data["Bounciness"];
		boxCollider->mFrictionCombine = data["FrictionCombine"];
		boxCollider->mBounceCombine = data["BounceCombine"];

		boxCollider->mCollider->mHalfSize = Vector3(data["Size"][0] / 2.0f, data["Size"][1] / 2.0f, data["Size"][2] / 2.0f);

		boxCollider->SynchronizeData();

		// 设置刚体的碰撞体和惯性张量(如果先解析RigidBody再解析Collider就会从这里设置)
		auto rigidBody = GetComponent<ZRigidBody>();
		if (rigidBody)
		{
			boxCollider->mCollider->mRigidBody = rigidBody->mRigidBody;
			rigidBody->mRigidBody->mCollisionVolume = boxCollider->mCollider;
			rigidBody->mRigidBody->SetInertiaTensor(boxCollider->mCollider->GetInertiaTensor(rigidBody->mRigidBody->GetMass()));
		}
	}

	void GameObject::ParsePlaneCollider(json data)
	{
		PlaneCollider* planeCollider = AddComponent<PlaneCollider>();

		mColliderType = PhysZ::ColliderType::Plane;

		planeCollider->mFriction = data["Friction"];
		planeCollider->mBounciness = data["Bounciness"];
		planeCollider->mFrictionCombine = data["FrictionCombine"];
		planeCollider->mBounceCombine = data["BounceCombine"];

		planeCollider->mCollider->mNormal = Vector3(data["Normal"][0], data["Normal"][1], data["Normal"][2]);
		planeCollider->mCollider->mDistance = data["Distance"];

		planeCollider->SynchronizeData();

		// 设置刚体的碰撞体和惯性张量(如果先解析RigidBody再解析Collider就会从这里设置)
		auto rigidBody = GetComponent<ZRigidBody>();
		if (rigidBody)
		{
			planeCollider->mCollider->mRigidBody = rigidBody->mRigidBody;
			rigidBody->mRigidBody->mCollisionVolume = planeCollider->mCollider;
			rigidBody->mRigidBody->SetInertiaTensor(planeCollider->mCollider->GetInertiaTensor(rigidBody->mRigidBody->GetMass()));
		}
	}

	void GameObject::ParseSphereCollider(json data)
	{
		SphereCollider* sphereCollider = AddComponent<SphereCollider>();

		mColliderType = PhysZ::ColliderType::Sphere;

		sphereCollider->mFriction = data["Friction"];
		sphereCollider->mBounciness = data["Bounciness"];
		sphereCollider->mFrictionCombine = data["FrictionCombine"];
		sphereCollider->mBounceCombine = data["BounceCombine"];

		sphereCollider->mCollider->mRadius = data["Radius"];

		sphereCollider->SynchronizeData();

		// 设置刚体的碰撞体和惯性张量(如果先解析RigidBody再解析Collider就会从这里设置)
		auto rigidBody = GetComponent<ZRigidBody>();
		if (rigidBody)
		{
			sphereCollider->mCollider->mRigidBody = rigidBody->mRigidBody;
			rigidBody->mRigidBody->mCollisionVolume = sphereCollider->mCollider;
			rigidBody->mRigidBody->SetInertiaTensor(sphereCollider->mCollider->GetInertiaTensor(rigidBody->mRigidBody->GetMass()));
		}
	}

	void GameObject::ParseRigidBody(json data)
	{
		ZRigidBody* rigidBody = AddComponent<ZRigidBody>();

		rigidBody->mUseGravity = data["UseGravity"];

		// 添加重力加速度
		if (rigidBody->mUseGravity)
		{
			auto fgGravity = new PhysZ::FGGravity(Vector3(0.0f, -9.8f, 0.0f));
			rigidBody->mRigidBody->AddForceGenerator(fgGravity);
		}

		if (data["InfiniteMass"] == true)
			rigidBody->mRigidBody->SetInverseMass(0.0f);
		else
			rigidBody->mRigidBody->SetMass(data["Mass"]);

		float damping = data["Damping"];
		rigidBody->mRigidBody->SetLinearDamping(1.0f - damping);

		float angularDamping = data["AngularDamping"];
		rigidBody->mRigidBody->SetAngularDamping(1.0f - angularDamping);

		// 初始化刚体的位置和旋转
		rigidBody->mRigidBody->SetPosition(GetComponent<Transform>()->GetPosition());
		rigidBody->mRigidBody->SetRotation(GetComponent<Transform>()->GetRotation());

		// 设置刚体的碰撞体和惯性张量(如果先解析Collider再解析RigidBody就会从这里设置)
		if (mColliderType == PhysZ::ColliderType::Box)
		{
			auto boxCollider = GetComponent<BoxCollider>();
			if (boxCollider)
			{
				boxCollider->mCollider->mRigidBody = rigidBody->mRigidBody;
				rigidBody->mRigidBody->mCollisionVolume = boxCollider->mCollider;
				rigidBody->mRigidBody->SetInertiaTensor(boxCollider->mCollider->GetInertiaTensor(rigidBody->mRigidBody->GetMass()));
			}
		}
		else if (mColliderType == PhysZ::ColliderType::Plane)
		{
			auto planeCollider = GetComponent<PlaneCollider>();
			if (planeCollider)
			{
				planeCollider->mCollider->mRigidBody = rigidBody->mRigidBody;
				rigidBody->mRigidBody->mCollisionVolume = planeCollider->mCollider;
				rigidBody->mRigidBody->SetInertiaTensor(planeCollider->mCollider->GetInertiaTensor(rigidBody->mRigidBody->GetMass()));
			}
		}
		else if (mColliderType == PhysZ::ColliderType::Sphere)
		{
			auto sphereCollider = GetComponent<SphereCollider>();
			if (sphereCollider)
			{
				sphereCollider->mCollider->mRigidBody = rigidBody->mRigidBody;
				rigidBody->mRigidBody->mCollisionVolume = sphereCollider->mCollider;
				rigidBody->mRigidBody->SetInertiaTensor(sphereCollider->mCollider->GetInertiaTensor(rigidBody->mRigidBody->GetMass()));
			}
		}
	}
}