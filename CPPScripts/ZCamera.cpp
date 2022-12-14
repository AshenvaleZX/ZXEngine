#include "ZCamera.h"
#include "RenderEngine.h"
#include "EventManager.h"
#include "Time.h"
#include "Transform.h"
#include "GlobalData.h"

namespace ZXEngine
{
	vector<Camera*> Camera::allCameras;
	vector<Camera*> Camera::GetAllCameras()
	{
		return allCameras;
	}

	ComponentType Camera::GetType()
	{
		return ComponentType::T_Camera;
	}

	Camera::Camera() : Fov(FOV)
	{
		allCameras.push_back(this);
	}

	Camera::~Camera()
	{
		auto l = std::find(allCameras.begin(), allCameras.end(), this);
		allCameras.erase(l);
	}

	ComponentType Camera::GetInsType()
	{
		return ComponentType::T_Camera;
	}
	
	Matrix4 Camera::GetViewMatrix()
	{
		// Model�����ǰѶ����ģ�Ϳռ�ת������ռ䣬�������View������ʵ���ǰѳ��������ж��������ռ�ת������Լ���ģ�Ϳռ�
		// ��������ֱ�ӷ���Model���������󼴿�
		// ������glm::lookAt����Math::GetLookToMatrixҲ��һ����
		Matrix4 model = GetTransform()->GetModelMatrix();
		return Math::Inverse(model);
	}

	Matrix4 Camera::GetProjectionMatrix()
	{
		return Math::Perspective(Math::Deg2Rad(Fov), (float)GlobalData::srcWidth / (float)GlobalData::srcHeight, nearClipDis, farClipDis);
	}
}