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

		// 窗口相关
		GLFWwindow* window;
		unsigned int scrWidth;
		unsigned int scrHeight;

		void InitWindow(unsigned int width, unsigned int height);
		void CloseWindow(string args);
		void SwapBufferAndPollPollEvents();
		int WindowShouldClose();

		// 绘制
		void BeginRender();
		void Render(Camera* camera);
		
	private:
		static RenderEngine* mInstance;
	};
}