#pragma once

namespace ZXEngine
{
	class Game
	{
	public:
		static Game* mInstance;

		Game();
		~Game();

		void Play();
	};
}