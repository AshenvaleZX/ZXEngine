#pragma once

class Debug
{
public:
	static void Log(string message)
	{
		cout << "Log: " << message << endl;
	}

	static void LogWarning(string message)
	{
		cout << "Warning: " << message << endl;
	}

	static void LogError(string message)
	{
		cout << "Error: " << message << endl;
	}
};