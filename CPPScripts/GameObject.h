#pragma once
#include "pubh.h"
#include "Resources.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/RectTransform.h"
#include "Component/MeshRenderer.h"
#include "Component/ZCamera.h"
#include "Component/Light.h"
#include "Component/GameLogic.h"
#include "Component/UIButton.h"
#include "Component/UITextRenderer.h"
#include "Component/UITextureRenderer.h"
#include "Component/ParticleSystem.h"
#include "Component/Physics/BoxCollider.h"
#include "Component/Physics/PlaneCollider.h"
#include "Component/Physics/SphereCollider.h"
#include "Component/Physics/ZRigidBody.h"
#include "Component/Physics/SpringJoint.h"
#include "Component/Physics/ZDistanceJoint.h"
#include "Component/Physics/Cloth.h"
#include "Component/Animator.h"
#include "Component/AudioSource.h"
#include "Component/AudioListener.h"
#include "PhysZ/PhysZEnumStruct.h"
#include "Util/Serialization.h"

namespace ZXEngine
{
	class GameObject
	{
		friend class EditorInspectorPanel;
	public:
		static GameObject* Create(const string& path);
		static GameObject* CreateInstance(const PrefabStruct* prefab);
		static void AsyncCreate(const string& path);
		static GameObject* Find(const string& path);

	private:
		static GameObject* Find(const vector<GameObject*>& gameObjects, const vector<string> paths, int& recursion);


	public:
		string name;
		uint32_t layer = 0;
		GameObject* parent = nullptr;
		vector<GameObject*> children;
		PhysZ::ColliderType mColliderType = PhysZ::ColliderType::None;

		GameObject() {};
		GameObject(const PrefabStruct* prefab, GameObject* parent = nullptr);
		~GameObject();

		void Awake();

		template<class T> 
		inline T* GetComponent() const;
		template<class T>
		inline T* AddComponent();

		bool IsActive() const;
		void SetActive(bool active);
		void AddChild(GameObject* child);
		void RemoveChild(GameObject* child, bool toRoot = true);
		void SetParent(GameObject* parent);
		GameObject* FindChildren(const string& path) const;
		void AddComponent(ComponentType type, Component* component);
		void EndConstruction();

	private:
		bool mActive = true;
		bool mIsAwake = false;
		multimap<ComponentType, Component*> components = {};
		vector<std::function<void()>> mConstructionCallBacks;

		// 通用解析器，使用反序列化接口自动解析
		template<class T>
		void Parse(json data)
		{
			T* component = AddComponent<T>();
			Serialization::DeserializeFromJson(*component, data);
		}

		void ParseTransform(json data);
		void ParseRectTransform(json data);
		void ParseMeshRenderer(json data, const ModelData* pModelData, MaterialStruct* material);
		void ParseLight(json data);
		void ParseUIButton(json data);
		void ParseUITextRenderer(json data);
		void ParseUITextureRenderer(json data);
		void ParseParticleSystem(json data);
		void ParseBoxCollider(json data);
		void ParsePlaneCollider(json data);
		void ParseSphereCollider(json data);
		void ParseRigidBody(json data);
		void ParseSpringJoint(json data);
		void ParseDistanceJoint(json data);
		void ParseCloth(json data);
		void ParseAudioSource(json data);
		void ParseAudioListener(json data);
	};

	template<class T>
	inline T* GameObject::GetComponent() const
	{
		ComponentType type = T::GetType();

		auto iter = components.find(type);
		if (iter != components.end())
		{
			return static_cast<T*>(iter->second);
		}

		if (ComponentChildLUT.count(type) != 0)
		{
			auto range = ComponentChildLUT.equal_range(type);
			for (auto& iter1 = range.first; iter1 != range.second; ++iter1)
			{
				auto iter2 = components.find(iter1->second);
				if (iter2 != components.end())
				{
					return static_cast<T*>(iter2->second);
				}
			}
		}

		return static_cast<T*>(nullptr);
	}

	template<class T>
	inline T* GameObject::AddComponent()
	{
		T* t = new T();
		t->gameObject = this;
		ComponentType type = T::GetType();

		if (Component::IsUnique(type) && components.count(type) > 0)
		{
			Debug::LogError("GameObject already has a component of type " + to_string((int)type));
		}
		
		components.insert(pair<ComponentType, Component*>(type, t));
		return t;
	}
}