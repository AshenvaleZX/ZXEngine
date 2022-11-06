#include "Transform.h"

namespace ZXEngine
{
	vec3 Transform::GetForward()
	{
		vec4 forward = rotation.ToMatrix() * vec4(0, 0, 1, 0);
		return vec3(forward.x, forward.y, forward.z);
	}

	vec3 Transform::GetRight()
	{
		vec4 right = rotation.ToMatrix() * vec4(1, 0, 0, 0);
		return vec3(right.x, right.y, right.z);
	}

	vec3 Transform::GetUp()
	{
		vec4 up = rotation.ToMatrix() * vec4(0, 1, 0, 0);
		return vec3(up.x, up.y, up.z);
	}
}