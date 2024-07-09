#include "GameObject.h"
#include "Material.h"
#include "ModelUtil.h"
#include "SceneManager.h"
#include "ZMesh.h"

namespace ZXEngine
{
	GameObject* GameObject::Create(const string& path)
	{
		auto prefab = Resources::LoadPrefab(path);
		auto gameObject = new GameObject(prefab);
		gameObject->EndConstruction();
		delete prefab;

		SceneManager::GetInstance()->GetCurScene()->AddGameObject(gameObject);

		gameObject->Awake();
		
		return gameObject;
	}

	GameObject* GameObject::CreateInstance(const PrefabStruct* prefab)
	{
		auto gameObject = new GameObject(prefab);
		gameObject->EndConstruction();
		SceneManager::GetInstance()->GetCurScene()->AddGameObject(gameObject);
		gameObject->Awake();
		return gameObject;
	}

	void GameObject::AsyncCreate(const string& path)
	{
		Resources::AsyncLoadPrefab(path, [](PrefabStruct* prefab)
		{
			auto gameObject = new GameObject(prefab);
			gameObject->EndConstruction();
			SceneManager::GetInstance()->GetCurScene()->AddGameObject(gameObject);
			gameObject->Awake();
		});
	}

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

	void GameObject::Awake()
	{
		if (mIsAwake)
			return;

		for (auto child : children)
			child->Awake();

		for (auto& iter : components)
			iter.second->Awake();

		mIsAwake = true;
	}

	bool GameObject::IsActive() const
	{
		return mActive;
	}

	void GameObject::SetActive(bool active)
	{
		mActive = active;
		for (auto child : children)
			child->SetActive(active);
	}

	void GameObject::AddChild(GameObject* child)
	{
		Vector3 pos = child->GetComponent<Transform>()->GetPosition();

		if (child->parent)
			child->parent->RemoveChild(child, false);
		else
			SceneManager::GetInstance()->GetCurScene()->RemoveGameObject(child);

		child->parent = this;
		child->GetComponent<Transform>()->SetPosition(pos);

		children.push_back(child);
	}

	void GameObject::RemoveChild(GameObject* child, bool toRoot)
	{
		auto iter = std::find(children.begin(), children.end(), child);
		if (iter != children.end())
		{
			if (toRoot)
			{
				Vector3 pos = child->GetComponent<Transform>()->GetPosition();

				(*iter)->parent = nullptr;
				(*iter)->GetComponent<Transform>()->SetPosition(pos);

				SceneManager::GetInstance()->GetCurScene()->AddGameObject(*iter);
			}
			else
			{
				(*iter)->parent = nullptr;
			}
			children.erase(iter);
		}
	}

	void GameObject::SetParent(GameObject* parent)
	{
		parent->AddChild(this);
	}

	GameObject* GameObject::FindChildren(const string& path) const
	{
		auto paths = Utils::StringSplit(path, '/');
		int recursion = 0;
		return GameObject::Find(children, paths, recursion);
	}

	GameObject::GameObject(const PrefabStruct* prefab, GameObject* parent)
	{
		name = prefab->name;
		layer = prefab->layer;
		this->parent = parent;

		for (auto& component : prefab->components)
		{
			if (component["Type"] == "Transform")
				ParseTransform(component);
			else if (component["Type"] == "RectTransform")
				ParseRectTransform(component);
			else if (component["Type"] == "MeshRenderer")
				ParseMeshRenderer(component, prefab->modelData, prefab->material);
			else if (component["Type"] == "Camera")
				ParseCamera(component);
			else if (component["Type"] == "Light")
				ParseLight(component);
			else if (component["Type"] == "GameLogic")
				ParseGameLogic(component);
			else if (component["Type"] == "UIButton")
				ParseUIButton(component);
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
			else if (component["Type"] == "SpringJoint")
				ParseSpringJoint(component);
			else if (component["Type"] == "DistanceJoint")
				ParseDistanceJoint(component);
			else if (component["Type"] == "Cloth")
				ParseCloth(component);
			else if (component["Type"] == "AudioSource")
				ParseAudioSource(component);
			else if (component["Type"] == "AudioListener")
				ParseAudioListener(component);
			else
				Debug::LogError("Try parse undefined component type: " + component["Type"]);
		}

		for (auto subPrefab : prefab->children)
		{
			auto subGameObject = new GameObject(subPrefab, this);
			children.push_back(subGameObject);
		}
	}

	GameObject::~GameObject()
	{
		if (parent)
			parent->RemoveChild(this, false);
		else
			SceneManager::GetInstance()->GetCurScene()->RemoveGameObject(this);

		for (auto& iter : components)
		{
			if (iter.first == ComponentType::Transform)
				delete static_cast<Transform*>(iter.second);
			else if (iter.first == ComponentType::RectTransform)
				delete static_cast<RectTransform*>(iter.second);
			else if (iter.first == ComponentType::MeshRenderer)
				delete static_cast<MeshRenderer*>(iter.second);
			else if (iter.first == ComponentType::Camera)
				delete static_cast<Camera*>(iter.second);
			else if (iter.first == ComponentType::Light)
				delete static_cast<Light*>(iter.second);
			else if (iter.first == ComponentType::GameLogic)
				delete static_cast<GameLogic*>(iter.second);
			else if (iter.first == ComponentType::UIButton)
				delete static_cast<UIButton*>(iter.second);
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
			else if (iter.first == ComponentType::SpringJoint)
				delete static_cast<SpringJoint*>(iter.second);
			else if (iter.first == ComponentType::DistanceJoint)
				delete static_cast<ZDistanceJoint*>(iter.second);
			else if (iter.first == ComponentType::Cloth)
				delete static_cast<Cloth*>(iter.second);
			else if (iter.first == ComponentType::AudioSource)
				delete static_cast<AudioSource*>(iter.second);
			else if (iter.first == ComponentType::AudioListener)
				delete static_cast<AudioListener*>(iter.second);
			else
				Debug::LogError("Try delete undefined component type: %s", static_cast<int>(iter.first));
		}

		for (int i = static_cast<int>(children.size()) - 1; i >= 0; i--)
			delete children[i];
	}

	void GameObject::AddComponent(ComponentType type, Component* component)
	{
		component->gameObject = this;
		components.insert(pair<ComponentType, Component*>(type, component));
	}

	void GameObject::EndConstruction()
	{
		for (auto& callback : mConstructionCallBacks)
			callback();

		mConstructionCallBacks.clear();

		for (auto child : children)
			child->EndConstruction();
	}

	void GameObject::ParseTransform(json data)
	{
		Transform* transform = AddComponent<Transform>();

		transform->SetLocalPosition(Vector3(data["Position"][0], data["Position"][1], data["Position"][2]));
		transform->SetLocalEulerAngles(data["Rotation"][0], data["Rotation"][1], data["Rotation"][2]);
		transform->SetLocalScale(Vector3(data["Scale"][0], data["Scale"][1], data["Scale"][2]));
	}

	void GameObject::ParseRectTransform(json data)
	{
		RectTransform* rectTransform = AddComponent<RectTransform>();

		rectTransform->SetLocalPosition(Vector3(data["Position"][0], data["Position"][1], data["Position"][2]));
		rectTransform->SetLocalEulerAngles(data["Rotation"][0], data["Rotation"][1], data["Rotation"][2]);
		rectTransform->SetLocalScale(Vector3(data["Scale"][0], data["Scale"][1], data["Scale"][2]));

		rectTransform->mAnchorV = data["VerticalAnchor"];
		rectTransform->mAnchorH = data["HorizontalAnchor"];
	}

	void GameObject::ParseMeshRenderer(json data, const ModelData* pModelData, MaterialStruct* material)
	{
		MeshRenderer* meshRenderer = AddComponent<MeshRenderer>();
		string p = "";

		meshRenderer->mCastShadow = data["CastShadow"];
		meshRenderer->mReceiveShadow = data["ReceiveShadow"];

		if (material)
		{
			meshRenderer->mMatetrial = new Material(material);
		}
		else
		{
			Debug::LogWarning("No material !");
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

			for (auto& mesh : pModelData->pMeshes)
			{
				mesh->SetUp();
			}
			meshRenderer->SetMeshes(pModelData->pMeshes);

			if (pModelData->pAnimationController)
			{
				Animator* animator = AddComponent<Animator>();
				animator->mAvatarName = meshRenderer->mModelName + "Avatar";
				animator->mRootBoneNode = pModelData->pRootBoneNode;
				animator->mAnimationController = pModelData->pAnimationController;
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

		if (!data["ShadowRange"].is_null())
			light->mDirectionalLightSpaceSize = data["ShadowRange"];
	}

	void GameObject::ParseGameLogic(json data)
	{
		GameLogic* gameLogic = AddComponent<GameLogic>();

		string p = Resources::JsonStrToString(data["Lua"]);
		gameLogic->luaName = Resources::GetAssetName(p);
		gameLogic->luaFullPath = Resources::GetAssetFullPath(p);
	}

	void GameObject::ParseUIButton(json data)
	{
		UIButton* uiButton = AddComponent<UIButton>();

		uiButton->mWidth = data["Width"];
		uiButton->mHeight = data["Height"];
	}

	void GameObject::ParseUITextRenderer(json data)
	{
		UITextRenderer* uiTextRenderer = AddComponent<UITextRenderer>();

		uiTextRenderer->size = data["Size"];
		uiTextRenderer->SetContent(data["Text"]);
		uiTextRenderer->SetHorizonAlignment(data["HorizonAlignment"]);
		uiTextRenderer->SetVerticalAlignment(data["VerticalAlignment"]);
		uiTextRenderer->color = Vector4(data["Color"][0], data["Color"][1], data["Color"][2], data["Color"][3]);
	}

	void GameObject::ParseUITextureRenderer(json data)
	{
		UITextureRenderer* uiTextureRenderer = AddComponent<UITextureRenderer>();

		string path = Resources::JsonStrToString(data["TexturePath"]);
		mConstructionCallBacks.push_back([=]()
		{
			uiTextureRenderer->SetTexture(Resources::GetAssetFullPath(path));
		});
	}

	void GameObject::ParseParticleSystem(json data)
	{
		ParticleSystem* particleSystem = AddComponent<ParticleSystem>();

		string p = Resources::JsonStrToString(data["TexturePath"]);
		particleSystem->SetTexture(Resources::GetAssetFullPath(p).c_str());
		particleSystem->mParticleNum = data["ParticleNum"];
		particleSystem->mLifeTime = data["LifeTime"];
		particleSystem->mVelocity = Vector3(data["Velocity"][0], data["Velocity"][1], data["Velocity"][2]);
		particleSystem->mOffset = Vector3(data["StartOffset"][0], data["StartOffset"][1], data["StartOffset"][2]);
		particleSystem->InternalGeneration();
	}

	void GameObject::ParseBoxCollider(json data)
	{
		BoxCollider* boxCollider = AddComponent<BoxCollider>();

		mColliderType = PhysZ::ColliderType::Box;

		boxCollider->mFriction = data["Friction"];
		boxCollider->mBounciness = data["Bounciness"];
		boxCollider->mFrictionCombine = data["FrictionCombine"];
		boxCollider->mBounceCombine = data["BounceCombine"];

		boxCollider->mCollider->mHalfSize = Vector3(static_cast<float>(data["Size"][0]) / 2.0f, static_cast<float>(data["Size"][1]) / 2.0f, static_cast<float>(data["Size"][2]) / 2.0f);

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

	void GameObject::ParseSpringJoint(json data)
	{
		SpringJoint* springJoint = AddComponent<SpringJoint>();

		springJoint->mConnectedGOPath = data["Connected"];
		springJoint->mRestLength = data["RestLength"];
		springJoint->mSpringConstant = data["SpringConstant"];
		springJoint->mAnchor = Vector3(data["Anchor"][0], data["Anchor"][1], data["Anchor"][2]);
		springJoint->mOtherAnchor = Vector3(data["OtherAnchor"][0], data["OtherAnchor"][1], data["OtherAnchor"][2]);

		mConstructionCallBacks.push_back([springJoint]()
		{
			springJoint->Init();
		});
	}

	void GameObject::ParseDistanceJoint(json data)
	{
		ZDistanceJoint* distanceJoint = AddComponent<ZDistanceJoint>();

		distanceJoint->mConnectedGOPath = data["Connected"];
		distanceJoint->mAnchor = Vector3(data["Anchor"][0], data["Anchor"][1], data["Anchor"][2]);
		distanceJoint->mOtherAnchor = Vector3(data["OtherAnchor"][0], data["OtherAnchor"][1], data["OtherAnchor"][2]);
		distanceJoint->mDistance = data["Distance"];

		mConstructionCallBacks.push_back([distanceJoint]()
		{
			distanceJoint->Init();
		});
	}

	void GameObject::ParseCloth(json data)
	{
		Cloth* cloth = AddComponent<Cloth>();

		mColliderType = PhysZ::ColliderType::Cloth;

		cloth->mUseGravity = data["UseGravity"];
		cloth->mMass = data["Mass"];
		cloth->mFriction = data["Friction"];
		cloth->mBendStiffness = data["BendStiffness"];
		cloth->mStretchStiffness = data["StretchStiffness"];

		mConstructionCallBacks.push_back([cloth]()
		{
			cloth->Init();
		});
	}

	void GameObject::ParseAudioSource(json data)
	{
		AudioSource* audioSource = AddComponent<AudioSource>();

		string p = Resources::JsonStrToString(data["Clip"]);
		audioSource->Init(p);
		audioSource->SetLoop(data["Loop"]);
		audioSource->SetVolume(data["Volume"]);
		audioSource->mPlayOnAwake = data["PlayOnAwake"];
		audioSource->mIs3D = data["Is3D"];

		if (!data["MinDistance"].is_null())
			audioSource->SetMinDistance(data["MinDistance"]);
		if (!data["MaxDistance"].is_null())
			audioSource->SetMaxDistance(data["MaxDistance"]);
	}

	void GameObject::ParseAudioListener(json data)
	{
		AddComponent<AudioListener>();
	}
}