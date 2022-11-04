#include "ZCamera.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Time.h"
#include "Transform.h"

namespace ZXEngine
{
	vector<Camera*> Camera::allCameras;

	Camera::Camera() : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
	{
		WorldUp = vec3(0.0f, 1.0f, 0.0f);
		Yaw = YAW;
		Pitch = PITCH;
		UpdateCameraVectors();

		allCameras.push_back(this);

		EventManager::GetInstance()->AddEventHandler(EventType::UPDATE_MOUSE_POS, std::bind(&Camera::MouseMoveCallBack, this, std::placeholders::_1));
		EventManager::GetInstance()->AddEventHandler(EventType::KEY_D_PRESS, std::bind(&Camera::MoveRightCallBack, this, std::placeholders::_1));
		EventManager::GetInstance()->AddEventHandler(EventType::KEY_A_PRESS, std::bind(&Camera::MoveLeftCallBack, this, std::placeholders::_1));
		EventManager::GetInstance()->AddEventHandler(EventType::KEY_S_PRESS, std::bind(&Camera::MoveDownCallBack, this, std::placeholders::_1));
		EventManager::GetInstance()->AddEventHandler(EventType::KEY_W_PRESS, std::bind(&Camera::MoveUpCallBack, this, std::placeholders::_1));
	}

	Camera::~Camera()
	{
		auto l = std::find(allCameras.begin(), allCameras.end(), this);
		allCameras.erase(l);
	}

	vector<Camera*> Camera::GetAllCameras()
	{
		return allCameras;
	}
	
	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	mat4 Camera::GetViewMatrix()
	{
		// return glm::lookAt(Position, Position + Front, Up);

		// ���³����Լ�ʵ��LookAt����
		// ����GLM��������Ϊ����ģ�ƽʱ�����ϵľ���������Ϊ����ģ��������￴��������ת�ù���һ������ʵ���viewMat������Ӧ���������ģ�
		//  Right.x,  Right.y,  Right.z, 0,
		//  Up.x,     Up.y,     Up.z,    0,
		// -Front.x, -Front.y, -Front.z, 0,
		//  0,        0,        0,       1
		// �����posMatͬ��
		vec3 pos = GetTransform()->position;
		mat4 viewMat = mat4(
			Right.x, Up.x, -Front.x, 0,
			Right.y, Up.y, -Front.y, 0,
			Right.z, Up.z, -Front.z, 0,
			0, 0, 0, 1);
		mat4 posMat = mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-pos.x, -pos.y, -pos.z, 1);

		return viewMat * posMat;
	}

	mat4 Camera::GetProjectionMatrix()
	{
		return perspective(radians(Fov), (float)RenderEngine::GetInstance()->scrWidth / (float)RenderEngine::GetInstance()->scrHeight, 0.1f, 100.0f);
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

	void Camera::MoveCamera(CameraMoveDir direction)
	{
		float velocity = MovementSpeed * Time::deltaTime;
		if (direction == CameraMoveDir::FORWARD)
			GetTransform()->position += Front * velocity;
		if (direction == CameraMoveDir::BACKWARD)
			GetTransform()->position -= Front * velocity;
		if (direction == CameraMoveDir::LEFT)
			GetTransform()->position -= Right * velocity;
		if (direction == CameraMoveDir::RIGHT)
			GetTransform()->position += Right * velocity;
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

	void Camera::MoveRightCallBack(string args)
	{
		MoveCamera(CameraMoveDir::RIGHT);
	}
	void Camera::MoveLeftCallBack(string args)
	{
		MoveCamera(CameraMoveDir::LEFT);
	}
	void Camera::MoveDownCallBack(string args)
	{
		MoveCamera(CameraMoveDir::BACKWARD);
	}
	void Camera::MoveUpCallBack(string args)
	{
		MoveCamera(CameraMoveDir::FORWARD);
	}
}