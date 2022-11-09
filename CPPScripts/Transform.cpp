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
		// ��ΪGLM�Ĺ��캯��ʵ�ַ�ʽ���к����Ƿ��ģ���������������б�ʾ�Ļ���xyzӦ���������ұ�һ��
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