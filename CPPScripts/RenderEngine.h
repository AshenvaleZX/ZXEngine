#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ZXEngine
{
	class RenderEngine
	{
	public:
		RenderEngine() {};
		~RenderEngine() {};

		static void InitWindow(unsigned int width, unsigned int height);
		static void SwapBufferAndPollPollEvents();
		static int WindowShouldClose();

	private:
		static GLFWwindow* window;

	};
}