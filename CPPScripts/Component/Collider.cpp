#include "Collider.h"

namespace ZXEngine
{
	ComponentType Collider::GetType()
	{
		return ComponentType::Collider;
	}

	ComponentType Collider::GetInsType()
	{
		return ComponentType::Collider;
	}
}