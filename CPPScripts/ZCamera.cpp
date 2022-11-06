#include "ZCamera.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Time.h"
#include "Transform.h"

namespace ZXEngine
{
	vector<Camera*> Camera::allCameras;

	Camera::Camera() : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
	{
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

		// 以下尝试自己实现LookAt函数
		// 由于GLM里是以列为主序的，平时书面上的矩阵都是以行为主序的，所以这里看起来像是转置过了一样，其实这个viewMat看起来应该是这样的：
		//  Right.x,  Right.y,  Right.z, 0,
		//  Up.x,     Up.y,     Up.z,    0,
		// -Front.x, -Front.y, -Front.z, 0,
		//  0,        0,        0,       1
		// 后面的posMat同理
		vec3 pos = GetTransform()->position;
		vec3 forward = GetTransform()->GetForward();
		vec3 right = GetTransform()->GetRight();
		vec3 up = GetTransform()->GetUp();
		mat4 viewMat = mat4(
			right.x, up.x, -forward.x, 0,
			right.y, up.y, -forward.y, 0,
			right.z, up.z, -forward.z, 0,
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

		vec3 eulerAngle = GetTransform()->rotation.GetEulerAngles();
		eulerAngle.x -= verticalOffset;
		eulerAngle.y += horizontalOffset;
		GetTransform()->rotation.SetEulerAngles(eulerAngle.x, eulerAngle.y, eulerAngle.z);
	}

	void Camera::MoveCamera(CameraMoveDir direction)
	{
		float velocity = MovementSpeed * Time::deltaTime;
		if (direction == CameraMoveDir::FORWARD)
			GetTransform()->position += GetTransform()->GetForward() * velocity;
		if (direction == CameraMoveDir::BACKWARD)
			GetTransform()->position -= GetTransform()->GetForward() * velocity;
		if (direction == CameraMoveDir::LEFT)
			GetTransform()->position -= GetTransform()->GetRight() * velocity;
		if (direction == CameraMoveDir::RIGHT)
			GetTransform()->position += GetTransform()->GetRight() * velocity;
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