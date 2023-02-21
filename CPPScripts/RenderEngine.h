#pragma once
#include "pubh.h"
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