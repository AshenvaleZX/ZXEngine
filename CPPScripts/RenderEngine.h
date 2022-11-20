#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ZXEngine
{
	class Mesh;
	class Shader;
	class Camera;
	class RenderEngine
	{
	public:
		RenderEngine() {};
		~RenderEngine() {};

		static void Create();
		static RenderEngine* GetInstance();

		// 窗口相关
		GLFWwindow* window;

		void InitSkyBox();
		void InitWindow(unsigned int width, unsigned int height);
		void CloseWindow(string args);
		int WindowShouldClose();

		// 绘制
		void BeginRender();
		void Render(Camera* camera);
		void EndRender();
		
	private:
		static RenderEngine* mInstance;

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		void SwapBufferAndPollPollEvents();
	};
}