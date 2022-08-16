#pragma once
#include "pubh.h"
#include "SceneManager.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ZXEngine
{
	class Game
	{
	public:
		static Game* mInstance;

		Game() {};
		~Game() {};

		void Play();
;		void Render();

	private:
	};
}