#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class Game
	{
	public:
		static bool Launch(const string& path);
		static void Run();
		static void Shutdown();

	private:
		static void Update();
		static void Render();
	};
}