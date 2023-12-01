#pragma once
#include "pubh.h"
#include <chrono>

namespace ZXEngine
{
	class Time
	{
	public:
		// 当前系统时间戳（秒）
		static long long curSysTime;
		// 当前系统时间戳（微秒）
		static long long curSysTime_micro;
		// 从游戏启动到当前的时间（秒）
		static float curTime;
		// 从上一帧到现在的时间
		static float deltaTime;
#ifdef ZX_EDITOR
		// 从编辑器启动到当前的时间（秒）
		static float curEditorTime;
#endif

		// 更新时间
		static void Update();
		static void UpdateCurTime();
	};
}