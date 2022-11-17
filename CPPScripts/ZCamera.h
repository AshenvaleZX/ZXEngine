#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
    // 一些相机参数的默认值
    const float SENSITIVITY = 0.1f;
    const float FOV = 45.0f;

	class Camera : public Component
	{
	public:
        // Camera options
        float MouseSensitivity;
        float Fov;

        Camera();
		~Camera();

        static vector<Camera*> GetAllCameras();

        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();
        // 根据水平和竖直偏移量调整相机视角
        void RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch = true);

        // 接收鼠标移动事件
        void MouseMoveCallBack(string args);

	private:
        // 处理镜头移动用的变量
        bool firstMouse = true;
        double lastX = 0;
        double lastY = 0;

        // 所有相机
        static vector<Camera*> allCameras;
	};
}