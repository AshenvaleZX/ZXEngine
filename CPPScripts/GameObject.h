#pragma once
#include "pubh.h"
#include "Component.h"
#include "Resources.h"

namespace ZXEngine
{
	class GameObject
	{
	public:
		unsigned int layer;

		GameObject(PrefabStruct* prefab);
		~GameObject() {};

		template<class T> 
		inline T* GetComponent(string type);
		template<class T>
		inline T* AddComponent(string type);

		void AddComponent(string type, Component* component);

	private:
		map<string, Component*> components = {};
	};

	template<class T>
	inline T* GameObject::GetComponent(string type)
	{
		map<string, Component*>::iterator iter = components.find(type);
		if (iter != components.end()) {
			return reinterpret_cast<T*> (iter->second);
		}
		else {
			return reinterpret_cast<T*> (nullptr);
		}
	}

	template<class T>
	inline T* GameObject::AddComponent(string type)
	{
		T* t = new T();
		t->gameObject = this;
		components.insert(pair<string, Component*>(type, t));
		return t;
	}
}