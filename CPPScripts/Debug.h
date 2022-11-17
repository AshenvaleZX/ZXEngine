#pragma once
#include <string>

namespace ZXEngine
{
	class Debug
	{
	public:
		static void Log(std::string message);
		static void LogWarning(std::string message);
		static void LogError(std::string message);
		static void Update();

	private:
		static int FPS;
		static long long lastSecond;
	};
}