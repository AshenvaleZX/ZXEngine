#pragma once
#include <string>

// 条件编译调试代码开关
// #define ZX_DEBUG

namespace ZXEngine
{
	class Debug
	{
	public:
		static void Log(std::string message);
		static void LogWarning(std::string message);
		static void LogError(std::string message);

#ifdef ZX_DEBUG
		static int drawCallCount;
		static void Update();
#endif
	};
}