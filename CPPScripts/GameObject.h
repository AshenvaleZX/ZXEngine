#pragma once
#include "pubh.h"
#include "Resources.h"
#include "Component/Component.h"
#include "Component/Transform.h"
#include "Component/MeshRenderer.h"
#include "Component/ZCamera.h"
#include "Component/Light.h"
#include "Component/GameLogic.h"
#include "Component/UITextRenderer.h"
#include "Component/UITextureRenderer.h"
#include "Component/ParticleSystem.h"
#include "Component/Physics/BoxCollider.h"
#include "Component/Physics/PlaneCollider.h"
#include "Component/Physics/SphereCollider.h"
#include "Component/Physics/ZRigidBody.h"
#include "Component/Physics/SpringJoint.h"
#include "Component/Physics/Cloth.h"
#include "Component/Animator.h"
#include "PhysZ/PhysZEnumStruct.h"

namespace ZXEngine
{
	class GameObject
	{
		friend class EditorInspectorPanel;
	public:
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
		GameObject(PrefabStruct* prefab, GameObject* parent = nullptr);
		~GameObject();

		template<class T> 
		inline T* GetComponent();
		template<class T>
		inline T* AddComponent();

		GameObject* FindChildren(const string& path);
		void AddComponent(ComponentType type, Component* component);
		void EndConstruction();

	private:
		map<ComponentType, Component*> components = {};
		vector<std::function<void()>> mConstructionCallBacks;

		void ParseTransform(json data);
		void ParseMeshRenderer(json data);
		void ParseCamera(json data);
		void ParseLight(json data);
		void ParseGameLogic(json data);
		void ParseUITextRenderer(json data);
		void ParseUITextureRenderer(json data);
		void ParseParticleSystem(json data);
		void ParseBoxCollider(json data);
		void ParsePlaneCollider(json data);
		void ParseSphereCollider(json data);
		void ParseRigidBody(json data);
		void ParseSpringJoint(json data);
		void ParseCloth(json data);
	};

	template<class T>
	inline T* GameObject::GetComponent()
	{
		ComponentType type = T::GetType();
		map<ComponentType, Component*>::iterator iter = components.find(type);
		if (iter != components.end()) {
			return static_cast<T*> (iter->second);
		}
		else {
			return static_cast<T*> (nullptr);
		}
	}

	template<class T>
	inline T* GameObject::AddComponent()
	{
		T* t = new T();
		t->gameObject = this;
		ComponentType type = T::GetType();
		components.insert(pair<ComponentType, Component*>(type, t));
		return t;
	}
}