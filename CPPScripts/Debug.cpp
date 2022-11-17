#include "Debug.h"
#include <iostream>
#include "Time.h"

namespace ZXEngine
{
	void Debug::Log(std::string message)
	{
		std::cout << "Log:     " << message << std::endl;
	}

	void Debug::LogWarning(std::string message)
	{
		std::cout << "Warning: " << message << std::endl;
	}

	void Debug::LogError(std::string message)
	{
		std::cout << "Error:   " << message << std::endl;
	}

	int Debug::FPS = 0;
	long long Debug::lastSecond = 0;
	void Debug::Update()
	{
		FPS++;
		if (lastSecond != Time::curTime)
		{
			Log("FPS: " + std::to_string(FPS));
			lastSecond = Time::curTime;
			FPS = 0;
		}
	}
}