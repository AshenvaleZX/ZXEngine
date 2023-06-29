#pragma once
#include <string>
#include <mutex>

// ����������Դ��뿪��
// #define ZX_DEBUG

namespace ZXEngine
{
	class Debug
	{
	public:
		static void Log(const std::string& message);
		static void LogWarning(const std::string& message);
		static void LogError(const std::string& message);

#ifdef ZX_DEBUG
		static int drawCallCount;
		static void Update();
#endif

	private:
		static std::mutex mWriteMutex;
		static void WriteToFile(const std::string& message);
	};
}