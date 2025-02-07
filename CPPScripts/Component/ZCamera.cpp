#include "ZCamera.h"
#include "Transform.h"
#include "../RenderEngine.h"
#include "../EventManager.h"
#include "../Time.h"
#include "../GlobalData.h"

namespace ZXEngine
{
	vector<Camera*> Camera::allCameras;

	ComponentType Camera::GetType()
	{
		return ComponentType::Camera;
	}

	Camera* Camera::GetMainCamera()
	{
		for (auto cam : allCameras)
		{
			if (cam->mCameraType == CameraType::GameCamera)
				return cam;
		}
		return nullptr;
	}

	vector<Camera*> Camera::GetAllCameras()
	{
		return allCameras;
	}

	Camera::Camera()
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
		float aspect = mAspect == 0.0f ? static_cast<float>(GlobalData::srcWidth) / static_cast<float>(GlobalData::srcHeight) : mAspect;
		auto res = Math::Perspective(Math::Deg2Rad(mFOV), aspect, mNearClipDis, mFarClipDis);

#if defined(ZX_PLATFORM_ANDROID)
		if (GlobalData::screenRotation == ScreenRotation::Rotate90)
			res = Math::Rotate(res, Math::PI * 0.5f, Vector3(0.0f, 0.0f, 1.0f));
		else if (GlobalData::screenRotation == ScreenRotation::Rotate180)
			res = Math::Rotate(res, Math::PI, Vector3(0.0f, 0.0f, 1.0f));
		else if (GlobalData::screenRotation == ScreenRotation::Rotate270)
			res = Math::Rotate(res, Math::PI * 1.5f, Vector3(0.0f, 0.0f, 1.0f));
#endif

		return res;
	}

	Matrix4 Camera::GetProjectionMatrixInverse() const
	{
		Matrix4 projection = GetProjectionMatrix();
		return Math::Inverse(projection);
	}

	Vector2 Camera::WorldToScreenPoint(const Vector3& wPos) const
	{
		Vector4 pos = GetProjectionMatrix() * GetViewMatrix() * wPos.ToPosVec4();
		pos = pos / pos.w;

#ifdef ZX_PLATFORM_ANDROID
		float x = pos.x;
		float y = pos.y;
		if (GlobalData::screenRotation == ScreenRotation::Rotate90)
		{
			pos.x = y;
			pos.y = -x;
		}
		else if (GlobalData::screenRotation == ScreenRotation::Rotate180)
		{
			pos.x = -x;
			pos.y = -y;
		}
		else if (GlobalData::screenRotation == ScreenRotation::Rotate270)
		{
			pos.x = -y;
			pos.y = x;
		}
#endif

		return Vector2
		(
			(pos.x + 1.0f) * 0.5f * static_cast<float>(GlobalData::srcWidth),
#ifdef ZX_API_VULKAN
			(pos.y + 1.0f) * 0.5f * static_cast<float>(GlobalData::srcHeight)
#else
			(1.0f - pos.y) * 0.5f * static_cast<float>(GlobalData::srcHeight)
#endif
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

#ifdef ZX_PLATFORM_ANDROID
		float x = pos.x;
		float y = pos.y;
		if (GlobalData::screenRotation == ScreenRotation::Rotate90)
		{
			pos.x = -y;
			pos.y = x;
		}
		else if (GlobalData::screenRotation == ScreenRotation::Rotate180)
		{
			pos.x = -x;
			pos.y = -y;
		}
		else if (GlobalData::screenRotation == ScreenRotation::Rotate270)
		{
			pos.x = y;
			pos.y = -x;
		}
#endif

		pos = GetProjectionMatrixInverse() * pos;
		pos = pos / pos.w;
		pos = GetViewMatrixInverse() * pos;

		Vector3 dir = pos - GetTransform()->GetPosition();
		dir.Normalize();

		return PhysZ::Ray(pos, dir);
	}
}