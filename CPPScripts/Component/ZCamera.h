#pragma once
#include "../pubh.h"
#include "Component.h"
#include "../PhysZ/Ray.h"

namespace ZXEngine
{
    const float FOV = 45.0f;

	class Camera : public Component
	{
    public:
        static ComponentType GetType();
        static vector<Camera*> GetAllCameras();

    private:
        // 所有相机
        static vector<Camera*> allCameras;

	public:
        float Fov;
        float aspect = 0.0f;
        float nearClipDis = 0.1f;
        float farClipDis = 200.0f;
        bool enableAfterEffects = false;
        CameraType cameraType;

        Camera();
		~Camera();

        virtual ComponentType GetInsType();

        Matrix4 GetViewMatrix();
        Matrix4 GetViewMatrixInverse();

        Matrix4 GetProjectionMatrix();
        Matrix4 GetProjectionMatrixInverse();

        PhysZ::Ray ScreenPointToRay(const Vector2& point);
	};
}