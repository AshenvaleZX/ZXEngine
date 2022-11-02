#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
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
		static GLFWwindow* window;
		static unsigned int scrWidth;
		static unsigned int scrHeight;

		static void InitWindow(unsigned int width, unsigned int height);
		static void SwapBufferAndPollPollEvents();
		static int WindowShouldClose();


		// 绘制
		void Render(Camera* camera);
		
	private:
		static RenderEngine* mInstance;
	};
}