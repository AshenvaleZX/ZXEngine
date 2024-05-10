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
	
	Matrix4 Camera::GetViewMatrix() const
	{
		// Model矩阵是把顶点从模型空间转到世界空间，而相机的View矩阵，其实就是把场景中所有顶点从世界空间转到相机自己的模型空间
		// 所以这里直接返回Model矩阵的逆矩阵即可
		// 这里用glm::lookAt或者Math::GetLookToMatrix也是一样的
		Matrix4 model = GetTransform()->GetModelMatrix();
		return Math::Inverse(model);
	}

	Matrix4 Camera::GetViewMatrixInverse() const
	{
		return GetTransform()->GetModelMatrix();
	}

	Matrix4 Camera::GetProjectionMatrix() const
	{
		if (aspect == 0.0f)
			return Math::Perspective(Math::Deg2Rad(Fov), static_cast<float>(GlobalData::srcWidth) / static_cast<float>(GlobalData::srcHeight), nearClipDis, farClipDis);
		else
			return Math::Perspective(Math::Deg2Rad(Fov), aspect, nearClipDis, farClipDis);
	}

	Matrix4 Camera::GetProjectionMatrixInverse() const
	{
		Matrix4 projection = GetProjectionMatrix();
		return Math::Inverse(projection);
	}

	Vector2 Camera::WorldToScreenPoint(const Vector3& point) const
	{
		Vector4 pos = GetProjectionMatrix() * GetViewMatrix() * point.ToPosVec4();
		pos = pos / pos.w;

		Debug::Log("Pos: %s", pos);
		return Vector2(
			(pos.x + 1.0f) * 0.5f * static_cast<float>(GlobalData::srcWidth),
			(1.0f - pos.y) * 0.5f * static_cast<float>(GlobalData::srcHeight)
		);
	}

	PhysZ::Ray Camera::ScreenPointToRay(const Vector2& point) const
	{
		Vector4 pos 
		{ 
			point.x / static_cast<float>(GlobalData::srcWidth) * 2.0f - 1.0f,
#ifdef ZX_API_VULKAN
			point.y / static_cast<float>(GlobalData::srcHeight) * 2.0f - 1.0f,
#else
			-(point.y / static_cast<float>(GlobalData::srcHeight) * 2.0f - 1.0f),
#endif
			-1.0f, 1.0f
		};

		pos = GetProjectionMatrixInverse() * pos;
		pos = pos / pos.w;
		pos = GetViewMatrixInverse() * pos;

		Vector3 dir = pos - GetTransform()->GetPosition();
		dir.Normalize();

		return PhysZ::Ray(pos, dir);
	}
}