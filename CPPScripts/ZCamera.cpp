#include "ZCamera.h"

namespace ZXEngine
{
	// 一些相机参数的默认值
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float FOV = 45.0f;

	Camera::Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	mat4 Camera::GetViewMatrix()
	{
		// return glm::lookAt(Position, Position + Front, Up);

		// 以下尝试自己实现LookAt函数
		// 由于GLM里是以列为主序的，平时书面上的矩阵都是以行为主序的，所以这里看起来像是转置过了一样，其实这个viewMat看起来应该是这样的：
		//  Right.x,  Right.y,  Right.z, 0,
		//  Up.x,     Up.y,     Up.z,    0,
		// -Front.x, -Front.y, -Front.z, 0,
		//  0,        0,        0,       1
		// 后面的posMat同理
		mat4 viewMat = mat4(
			Right.x, Up.x, -Front.x, 0,
			Right.y, Up.y, -Front.y, 0,
			Right.z, Up.z, -Front.z, 0,
			0, 0, 0, 1);
		mat4 posMat = mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-Position.x, -Position.y, -Position.z, 1);

		return viewMat * posMat;
	}

	mat4 Camera::GetProjectionMatrix()
	{
		return perspective(radians(Fov), (float)RenderEngine::scrWidth / (float)RenderEngine::scrHeight, 0.1f, 100.0f);
	}

	void Camera::Render(list<GameObject*> gameObjects)
	{
		for (auto go : gameObjects)
		{
			MeshRenderer* meshRenderer = go->GetComponent<MeshRenderer>("MeshRenderer");
		}

	}

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void Camera::UpdateCameraVectors()
	{
		// Calculate the new Front vector
		vec3 front;
		front.x = cos(radians(Yaw)) * cos(radians(Pitch));
		front.y = sin(radians(Pitch));
		front.z = sin(radians(Yaw)) * cos(radians(Pitch));
		Front = normalize(front);
		// Also re-calculate the Right and Up vector
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Right = normalize(cross(Front, WorldUp));
		Up = normalize(cross(Right, Front));
	}
}