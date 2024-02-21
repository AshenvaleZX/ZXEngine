#include "Component.h"
#include "../GameObject.h"

namespace ZXEngine
{
	ComponentType Component::GetType()
	{
		return ComponentType::Component;
	}

	void Component::Awake() 
	{ 
		mIsAwake = true; 
	};

	Transform* Component::GetTransform()
	{
		return gameObject->GetComponent<Transform>();
	}
}