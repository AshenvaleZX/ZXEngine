#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

namespace ZXEngine
{
	// ��ǰ���������include�������ѭ��include
	class Camera;
	class GameObject;
	class RenderAPI;
	class RenderEngine
	{
	public:
		RenderEngine() {};
		~RenderEngine() {};
		static RenderEngine* mInstance;

		// �������
		static unsigned int scrWidth;
		static unsigned int scrHeight;

		static void InitWindow(unsigned int width, unsigned int height);
		static void SwapBufferAndPollPollEvents();
		static int WindowShouldClose();


		// ����
		void Render(Camera* camera, GameObject* gameObject);
		
	private:
		static GLFWwindow* window;

	};
}