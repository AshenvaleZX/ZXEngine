#include "ZCamera.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Time.h"
#include "Transform.h"

namespace ZXEngine
{
	vector<Camera*> Camera::allCameras;

	Camera::Camera() : Fov(FOV)
	{
		allCameras.push_back(this);
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
}