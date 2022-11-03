#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ZCamera.h"

namespace ZXEngine
{
	class RenderEngine
	{
	public:
		RenderEngine() {};
		~RenderEngine() {};

		static void Create();
		static RenderEngine* GetInstance();

		// �������
		GLFWwindow* window;
		unsigned int scrWidth;
		unsigned int scrHeight;

		void InitWindow(unsigned int width, unsigned int height);
		void CloseWindow(string args);
		int WindowShouldClose();

		// ����
		void BeginRender();
		void Render(Camera* camera);
		void EndRender();
		
	private:
		static RenderEngine* mInstance;

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		void SwapBufferAndPollPollEvents();
	};
}