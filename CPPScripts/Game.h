#pragma once
#include "pubh.h"
#include "SceneManager.h"

namespace ZXEngine
{
	class Game
	{
	public:
		static Game* mInstance;

		Game();
		~Game();

		void Play();
		void Render();
	};
}