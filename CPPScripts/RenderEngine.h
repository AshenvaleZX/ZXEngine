#pragma once
#include "pubh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "ZCamera.h"
#include "GameObject.h"

namespace ZXEngine
{
	class Camera;
	class RenderEngine
	{
	public:
		RenderEngine() {};
		~RenderEngine() {};

		static unsigned int scrWidth;
		static unsigned int scrHeight;
		static RenderEngine* mInstance;

		static void InitWindow(unsigned int width, unsigned int height);
		static void SwapBufferAndPollPollEvents();
		static int WindowShouldClose();
		static unsigned int LoadTexture(const char* path);
		static unsigned int LoadAndCompileShader(const char* path);

		void Render(Camera* camera, GameObject* gameObject);

	private:
		static GLFWwindow* window;

		static void CheckCompileErrors(unsigned int shader, std::string type);

	};
}