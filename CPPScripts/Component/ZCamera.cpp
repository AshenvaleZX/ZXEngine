#include "ZCamera.h"
#include "Transform.h"
#include "../RenderEngine.h"
#include "../EventManager.h"
#include "../Time.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	vector<Camera*> Camera::allCameras;
	vector<Camera*> Camera::GetAllCameras()
	{
		return allCameras;
	}

	ComponentType Camera::GetType()
	{
		return ComponentType::Camera;
	}

	Camera::Camera() : Fov(FOV)
	{
		cameraType = CameraType::GameCamera;
		allCameras.push_back(this);
	}

	Camera::~Camera()
	{
		auto l = std::find(allCameras.begin(), allCameras.end(), this);
		allCameras.erase(l);
	}

	ComponentType Camera::GetInsType()
	{
		return ComponentType::Camera;
	}
	
	Matrix4 Camera::GetViewMatrix()
	{
		// Model�����ǰѶ����ģ�Ϳռ�ת������ռ䣬�������View������ʵ���ǰѳ��������ж��������ռ�ת������Լ���ģ�Ϳռ�
		// ��������ֱ�ӷ���Model���������󼴿�
		// ������glm::lookAt����Math::GetLookToMatrixҲ��һ����
		Matrix4 model = GetTransform()->GetModelMatrix();
		return Math::Inverse(model);
	}

	Matrix4 Camera::GetViewMatrixInverse()
	{
		return GetTransform()->GetModelMatrix();
	}

	Matrix4 Camera::GetProjectionMatrix()
	{
		if (aspect == 0.0f)
			return Math::Perspective(Math::Deg2Rad(Fov), static_cast<float>(GlobalData::srcWidth) / static_cast<float>(GlobalData::srcHeight), nearClipDis, farClipDis);
		else
			return Math::Perspective(Math::Deg2Rad(Fov), aspect, nearClipDis, farClipDis);
	}

	Matrix4 Camera::GetProjectionMatrixInverse()
	{
		Matrix4 projection = GetProjectionMatrix();
		return Math::Inverse(projection);
	}
}