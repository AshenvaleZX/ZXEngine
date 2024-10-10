#pragma once
#include "../pubh.h"
#include "Component.h"
#include "../PhysZ/Ray.h"
#include "../Reflection/StaticReflection.h"

namespace ZXEngine
{
	class Camera : public Component
	{
    public:
        static ComponentType GetType();
        static Camera* GetMainCamera();
        static vector<Camera*> GetAllCameras();

    private:
        // 所有相机
        static vector<Camera*> allCameras;

	public:
        float mFOV = 45.0f;
        float mAspect = 0.0f;
        float mNearClipDis = 0.1f;
        float mFarClipDis = 200.0f;
        bool mEnableAfterEffects = false;
        CameraType mCameraType = CameraType::GameCamera;

        Camera();
		~Camera();

        virtual ComponentType GetInsType();

        Matrix4 GetViewMatrix() const;
        Matrix4 GetViewMatrixInverse() const;

        Matrix4 GetProjectionMatrix() const;
        Matrix4 GetProjectionMatrixInverse() const;

        Vector2 WorldToScreenPoint(const Vector3& point) const;
        PhysZ::Ray ScreenPointToRay(const Vector2& point) const;
	};

    StaticReflection
    (
        Camera,
        Fields
        (
            Field(&Camera::mFOV),
            Field(&Camera::mNearClipDis),
            Field(&Camera::mFarClipDis),
            Field(&Camera::mEnableAfterEffects)
        )
    )
}