#include "ZCamera.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Time.h"
#include "Transform.h"
#include "GlobalData.h"

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
		vec3 pos = GetTransform()->position;
		vec3 forward = GetTransform()->GetForward();
		vec3 up = GetTransform()->GetUp();
		return Utils::GetLookToMatrix(pos, forward, up);
	}

	mat4 Camera::GetProjectionMatrix()
	{
		return perspective(radians(Fov), (float)GlobalData::srcWidth / (float)GlobalData::srcHeight, nearClipDis, farClipDis);
	}
}