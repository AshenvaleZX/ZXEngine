#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
    const float FOV = 45.0f;

	class Camera : public Component
	{
	public:
        float Fov;
        float nearClipDis = 0.1f;
        float farClipDis = 200.0f;

        Camera();
		~Camera();

        static vector<Camera*> GetAllCameras();

        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();

	private:
        // 所有相机
        static vector<Camera*> allCameras;
	};
}