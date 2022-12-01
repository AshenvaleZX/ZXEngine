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

	ComponentType Camera::GetType()
	{
		return ComponentType::T_Camera;
	}
	
	mat4 Camera::GetViewMatrix()
	{
		// Model矩阵是把顶点从模型空间转到世界空间，而相机的View矩阵，其实就是把场景中所有顶点从世界空间转到相机自己的模型空间
		// 所以这里直接返回Model矩阵的逆矩阵即可
		// 这里用glm::lookAt或者Utils::GetLookToMatrix也是一样的
		mat4 model = GetTransform()->GetModelMatrix();
		return inverse(model);
	}

	mat4 Camera::GetProjectionMatrix()
	{
		return Math::Perspective(radians(Fov), (float)GlobalData::srcWidth / (float)GlobalData::srcHeight, nearClipDis, farClipDis);
	}
}