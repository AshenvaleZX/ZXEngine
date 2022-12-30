#pragma once
#include "pubh.h"
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "ZCamera.h"
#include "Light.h"
#include "GameLogic.h"
#include "UITextRenderer.h"
#include "UITextureRenderer.h"
#include "Resources.h"
#include "ParticleSystem.h"

namespace ZXEngine
{
	class GameObject
	{
#ifdef ZX_EDITOR
		friend class EditorInspectorPanel;
#endif
	public:
		string name;
		unsigned int layer;
		GameObject* parent = nullptr;
		vector<GameObject*> children;

		GameObject(PrefabStruct* prefab);
		~GameObject() {};

		template<class T> 
		inline T* GetComponent();
		template<class T>
		inline T* AddComponent();

		void AddComponent(ComponentType type, Component* component);

	private:
		map<ComponentType, Component*> components = {};
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