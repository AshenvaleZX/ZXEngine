#pragma once
#include "pubh.h"
#include <glad/glad.h>
#ifdef ZX_API_VULKAN
// 用GLFW的话这里就不要自己去include Vulkan的头文件，用这个宏定义，让GLFW自己去处理，不然有些接口有问题
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

		// 窗口相关
		GLFWwindow* window;

		void InitWindow();
		void CloseWindow(string args);
		int WindowShouldClose();

		// 绘制
		void BeginRender();
		void Render(Camera* camera);
		void EndRender();
		
	private:
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		void SwapBufferAndPollPollEvents();
	};
}