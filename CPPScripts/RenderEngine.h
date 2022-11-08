#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ZCamera.h"

namespace ZXEngine
{
	class Mesh;
	class Shader;
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

		void InitSkyBox();
		void InitWindow(unsigned int width, unsigned int height);
		void CloseWindow(string args);
		int WindowShouldClose();

		// 绘制
		void BeginRender();
		void Render(Camera* camera);
		void EndRender();
		void RenderSkyBox();
		
	private:
		static RenderEngine* mInstance;

		Mesh* skyBox;
		Shader* skyBoxShader;

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		void SwapBufferAndPollPollEvents();
	};
}