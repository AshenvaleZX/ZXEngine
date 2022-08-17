#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

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
		static unsigned int LoadTexture(const char* path);
		static unsigned int LoadAndCompileShader(const char* path);

	private:
		static GLFWwindow* window;

		static void CheckCompileErrors(unsigned int shader, std::string type);

	};
}