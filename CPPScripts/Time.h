#pragma once
#include <chrono>

namespace ZXEngine
{
	class Time
	{
	public:
		// 当前系统时间戳（秒）
		static long long curTime;
		// 当前系统时间戳（毫秒）
		static long long curTime_milli;
		// 当前系统时间戳（微秒）
		static long long curTime_micro;
		static float deltaTime;
		// 更新时间
		static void Update();
	};
}