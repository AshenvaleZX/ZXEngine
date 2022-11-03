#include "Component.h"
#include "GameObject.h"

namespace ZXEngine
{
	Transform* Component::GetTransform()
	{
		return gameObject->GetComponent<Transform>("Transform");
	}
}