#pragma once
#include "pubh.h"
#include "Resources.h"

namespace ZXEngine
{
    // 一些相机参数的默认值
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float FOV = 45.0f;

	class Camera
	{
	public:
        // Camera Attributes
        vec3 Position;
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

        Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
        Camera(CameraStruct* camStruct);
		~Camera() {};

        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();
        // 接收鼠标移动事件
        void MouseMoveCallBack(string args);
        // 根据水平和竖直偏移量调整相机视角
        void RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch = true);

	private:
        // 处理镜头移动用的变量
        bool firstMouse = true;
        double lastX = 0;
        double lastY = 0;

        void UpdateCameraVectors();

	};
}