#pragma once
#include "pubh.h"
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"

namespace ZXEngine
{
	class GameObject
	{
	public:
		GameObject() {};
		~GameObject() {};

		template<class T> 
		inline T* GetComponent(string type);

	private:
		Transform* transform = new Transform();
		MeshRenderer* meshRenderer = new MeshRenderer();
		map<string, Component*> components = { {"Transform", transform}, {"MeshRenderer", meshRenderer} };
	};

	template<class T>
	inline T* GameObject::GetComponent(string type)
	{
		map<string, Component*>::iterator iter = components.find(type);
		if (iter != components.end()) {
			return static_cast<T*> (iter->second);
		}
		else {
			return static_cast<T*> (nullptr);
		}
	}
}