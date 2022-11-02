#include "ZCamera.h"
#include "RenderEngine.h"
#include "EventManager.h"

namespace ZXEngine
{
	Camera::Camera(vec3 position, vec3 up, float yaw, float pitch) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		UpdateCameraVectors();
	}

	Camera::Camera(CameraStruct* camStruct) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
	{
		Position = camStruct->position;
		WorldUp = vec3(0.0f, 1.0f, 0.0f);
		Yaw = YAW;
		Pitch = PITCH;
		UpdateCameraVectors();
		EventManager::GetInstance()->AddEventHandler(EventType::UPDATE_MOUSE_POS, std::bind(&Camera::MouseMoveCallBack, this, std::placeholders::_1));
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
		return perspective(radians(Fov), (float)RenderEngine::GetInstance()->scrWidth / (float)RenderEngine::GetInstance()->scrHeight, 0.1f, 100.0f);
	}

	void Camera::MouseMoveCallBack(string args)
	{
		vector<string> argList = Utils::StringSplit(args, '|');
		double xpos = atof(argList[0].c_str());
		double ypos = atof(argList[1].c_str());
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		RotateAngleOfView(xoffset, yoffset);
	}

	void Camera::RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch)
	{
		horizontalOffset *= MouseSensitivity;
		verticalOffset *= MouseSensitivity;

		Yaw += horizontalOffset;
		Pitch += verticalOffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		UpdateCameraVectors();
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