#pragma once
#include "pubh.h"
#include "Component.h"

namespace ZXEngine
{
    // ����ƶ�����
    enum CameraMoveDir
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    // һЩ���������Ĭ��ֵ
    const float SPEED = 2.5f;
    const float SENSITIVITY = 0.1f;
    const float FOV = 45.0f;

	class Camera : public Component
	{
	public:
        // Camera options
        float MovementSpeed;
        float MouseSensitivity;
        float Fov;

        Camera();
		~Camera();

        static vector<Camera*> GetAllCameras();

        mat4 GetViewMatrix();
        mat4 GetProjectionMatrix();
        // ����ˮƽ����ֱƫ������������ӽ�
        void RotateAngleOfView(float horizontalOffset, float verticalOffset, bool constrainPitch = true);
        // �ƶ����λ��
        void MoveCamera(CameraMoveDir direction);

        // ��������ƶ��¼�
        void MouseMoveCallBack(string args);
        // ���ռ��̰����¼�
        void MoveRightCallBack(string args);
        void MoveLeftCallBack(string args);
        void MoveDownCallBack(string args);
        void MoveUpCallBack(string args);

	private:
        // ����ͷ�ƶ��õı���
        bool firstMouse = true;
        double lastX = 0;
        double lastY = 0;

        // �������
        static vector<Camera*> allCameras;
	};
}