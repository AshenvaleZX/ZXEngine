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

namespace ZXEngine
{
	class GameObject
	{
		friend class EditorInspectorPanel;
	public:
		string name;
		unsigned int layer = 0;
		GameObject* parent = nullptr;
		vector<GameObject*> children;

		GameObject() {};
		GameObject(PrefabStruct* prefab);
		~GameObject();

		template<class T> 
		inline T* GetComponent();
		template<class T>
		inline T* AddComponent();

		void AddComponent(ComponentType type, Component* component);

	private:
		map<ComponentType, Component*> components = {};

		void ParseTransform(json data);
		void ParseMeshRenderer(json data);
		void ParseCamera(json data);
		void ParseLight(json data);
		void ParseGameLogic(json data);
		void ParseUITextRenderer(json data);
		void ParseUITextureRenderer(json data);
		void ParseParticleSystem(json data);
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