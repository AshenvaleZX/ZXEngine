#include "Component.h"
#include "GameObject.h"

namespace ZXEngine
{
	ComponentType Component::GetType()
	{
		return ComponentType::T_Component;
	}

	Transform* Component::GetTransform()
	{
		return gameObject->GetComponent<Transform>();
	}
}