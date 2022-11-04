#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
    // 相机移动方向
    enum CameraMoveDir
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // 一些相机参数的默认值
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float FOV = 45.0f;

	class Camera : public Component
	{
	public:
        // Camera Attributes
        vec3 Front;
        vec3 Up;
        vec3 Right;
        vec3 WorldUp;
        // Euler Angles
        float Yaw;
        float Pitch;
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        Camera();
		~Camera();

        static vector<Camera*> GetAllCameras();

        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();
        // 根据水平和竖直偏移量调整相机视角
        void RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch = true);
        // 移动相机位置
        void MoveCamera(CameraMoveDir direction);

        // 接收鼠标移动事件
        void MouseMoveCallBack(string args);
        // 接收键盘按键事件
        void MoveRightCallBack(string args);
        void MoveLeftCallBack(string args);
        void MoveDownCallBack(string args);
        void MoveUpCallBack(string args);

	private:
        // 处理镜头移动用的变量
        bool firstMouse = true;
        double lastX = 0;
        double lastY = 0;

        // 所有相机
        static vector<Camera*> allCameras;

        void UpdateCameraVectors();

	};
}