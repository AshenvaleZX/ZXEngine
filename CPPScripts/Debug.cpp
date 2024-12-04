#include "Debug.h"
#include <iostream>
#include "PublicEnum.h"
#include "ProjectSetting.h"
#include "Editor/EditorDataManager.h"

namespace ZXEngine
{
	/// <summary>
	/// 日志模块
	/// </summary>
	
	std::mutex Debug::mWriteMutex;

	void Debug::Log(const std::string& message)
	{
		std::cout << "Log:     " << message << std::endl;

		if (ProjectSetting::logToFile)
		{
			WriteToFile("Log:     " + message);
		}

#ifdef ZX_EDITOR
		EditorDataManager::GetInstance()->AddLog(LogType::Message, message);
#endif
	}

	void Debug::LogWarning(const std::string& message)
	{
		std::cout << "Warning: " << message << std::endl;

		if (ProjectSetting::logToFile)
		{
			WriteToFile("Warning: " + message);
		}

#ifdef ZX_EDITOR
		EditorDataManager::GetInstance()->AddLog(LogType::Warning, message);
#endif
	}

	void Debug::LogError(const std::string& message)
	{
		std::cout << "Error:   " << message << std::endl;

		if (ProjectSetting::logToFile)
		{
			WriteToFile("Error:   " + message);
		}

#ifdef ZX_EDITOR
		EditorDataManager::GetInstance()->AddLog(LogType::Error, message);
#endif
	}

#ifdef ZX_DEBUG
	int Debug::drawCallCount;
	void Debug::Update() 
	{
		Log("Draw Call: " + std::to_string(drawCallCount));
		drawCallCount = 0;
	}
#endif

	void Debug::WriteToFile(const std::string& message)
	{
		std::lock_guard lock(mWriteMutex);

		std::ofstream f(ProjectSetting::projectPath + "/log.txt", std::ios::app);

		if (!f.is_open())
		{
			std::cerr << "Open log file failed !" << std::endl;
		}

		f << message << std::endl;

		f.close();
	}

	void Debug::Replace(std::string& message, const std::string& from, const std::string& to)
	{
		size_t pos = 0;
		if ((pos = message.find(from, pos)) != std::string::npos)
		{
			message.replace(pos, from.length(), to);
		}
	}
	
	/// <summary>
	/// 计时器模块
	/// </summary>
	
	size_t Debug::mTimerStackTop = 0;
	std::chrono::steady_clock::time_point Debug::mTimerStack[];

	std::unordered_map<std::string, std::chrono::steady_clock::time_point> Debug::mTimerMap;

	void Debug::PushTimer()
	{
		if (mTimerStackTop >= mTimerStackSize)
		{
			LogError("Timer stack overflow !");
			return;
		}

		mTimerStack[mTimerStackTop++] = std::chrono::steady_clock::now();
	}

	void Debug::PopTimer(const std::string& name)
	{
		if (mTimerStackTop == 0)
		{
			LogError("Timer stack underflow !");
			return;
		}

		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - mTimerStack[--mTimerStackTop]).count();

		// 没有用Log，因为Log速度慢一倍，会影响计时，如果有需要写入文件再用Log
		// 先拼接再输出，效率比直接用<<高
		cout << (name + " : " + std::to_string(duration) + " ns") << endl;
	}

	void Debug::StartTimer(const std::string& name)
	{
		mTimerMap[name] = std::chrono::steady_clock::now();
	}

	void Debug::EndTimer(const std::string& name)
	{
		auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - mTimerMap[name]).count();

		cout << (name + " : " + std::to_string(duration) + " ns") << endl;
	}
}