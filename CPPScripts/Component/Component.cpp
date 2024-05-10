#include "Component.h"
#include "../GameObject.h"

namespace ZXEngine
{
	ComponentType Component::GetType()
	{
		return ComponentType::Component;
	}

	bool Component::IsUnique(ComponentType type)
	{
		return !(type == ComponentType::SpringJoint || type == ComponentType::DistanceJoint);
	}

	void Component::Awake() 
	{ 
		mIsAwake = true; 
	};

	bool Component::IsActive() const
	{
		return mEnabled && gameObject->mActive;
	}

	Transform* Component::GetTransform() const
	{
		return gameObject->GetComponent<Transform>();
	}
}