#include "Transform.h"

namespace ZXEngine
{
	mat4 Transform::GetModelMatrix()
	{
		return GetPositionMatrix() * GetRotationMatrix() * GetScaleMatrix();
	}

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

	mat4 Transform::GetPositionMatrix()
	{
		// 因为GLM的构造函数实现方式，行和列是反的，按正常矩阵的行列表示的话，xyz应该是在最右边一列
		return mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			position.x, position.y, position.z, 1);
	}

	mat4 Transform::GetRotationMatrix()
	{
		return rotation.ToMatrix();
	}

	mat4 Transform::GetScaleMatrix()
	{
		return mat4(
			scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1);
	}
}