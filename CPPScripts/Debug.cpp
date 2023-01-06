#include "Debug.h"
#include <iostream>
#include "PublicEnum.h"
#include "Editor/EditorDataManager.h"

namespace ZXEngine
{
	void Debug::Log(std::string message)
	{
		std::cout << "Log:     " << message << std::endl;
#ifdef ZX_EDITOR
		EditorDataManager::GetInstance()->AddLog(LogType::Message, message);
#endif
	}

	void Debug::LogWarning(std::string message)
	{
		std::cout << "Warning: " << message << std::endl;
#ifdef ZX_EDITOR
		EditorDataManager::GetInstance()->AddLog(LogType::Warning, message);
#endif
	}

	void Debug::LogError(std::string message)
	{
		std::cout << "Error:   " << message << std::endl;
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
}