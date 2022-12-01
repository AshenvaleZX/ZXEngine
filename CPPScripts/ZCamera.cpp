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
		// Model�����ǰѶ����ģ�Ϳռ�ת������ռ䣬�������View������ʵ���ǰѳ��������ж��������ռ�ת������Լ���ģ�Ϳռ�
		// ��������ֱ�ӷ���Model���������󼴿�
		// ������glm::lookAt����Utils::GetLookToMatrixҲ��һ����
		mat4 model = GetTransform()->GetModelMatrix();
		return inverse(model);
	}

	mat4 Camera::GetProjectionMatrix()
	{
		return Math::Perspective(radians(Fov), (float)GlobalData::srcWidth / (float)GlobalData::srcHeight, nearClipDis, farClipDis);
	}
}