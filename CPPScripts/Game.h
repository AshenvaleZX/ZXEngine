#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Game
	{
	public:
		static Game* mInstance;

		Game() {};
		~Game() {};

		void Play();
		void Update();
;		void Render();

	private:
	};
}