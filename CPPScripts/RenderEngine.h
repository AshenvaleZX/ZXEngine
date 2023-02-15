#pragma once
#include "pubh.h"
#include <glad/glad.h>
#ifdef ZX_API_VULKAN
// ��GLFW�Ļ�����Ͳ�Ҫ�Լ�ȥinclude Vulkan��ͷ�ļ���������궨�壬��GLFW�Լ�ȥ������Ȼ��Щ�ӿ�������
#define GLFW_INCLUDE_VULKAN
#endif
#include <GLFW/glfw3.h>

namespace ZXEngine
{
	class Camera;
	class RenderEngine
	{
	public:
		static void Create();
		static RenderEngine* GetInstance();

	private:
		static RenderEngine* mInstance;

	public:
		RenderEngine();
		~RenderEngine() {};

		// �������
		GLFWwindow* window;

		void InitWindow();
		void CloseWindow(string args);
		int WindowShouldClose();

		// ����
		void BeginRender();
		void Render(Camera* camera);
		void EndRender();
		
	private:
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		void SwapBufferAndPollPollEvents();
	};
}