#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
    // һЩ���������Ĭ��ֵ
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
        // ����ˮƽ����ֱƫ������������ӽ�
        void RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch = true);

        // ��������ƶ��¼�
        void MouseMoveCallBack(string args);

	private:
        // ����ͷ�ƶ��õı���
        bool firstMouse = true;
        double lastX = 0;
        double lastY = 0;

        // �������
        static vector<Camera*> allCameras;
	};
}