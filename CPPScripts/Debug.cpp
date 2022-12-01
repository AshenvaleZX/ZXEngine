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
}