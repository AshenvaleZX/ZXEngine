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
		// 从游戏启动到当前的时间（微秒）
		static long long curTime_micro;
		// 从上一帧到现在的时间（秒）
		static float deltaTime;
		// 从上一帧到现在的时间（微秒）
		static long long deltaTime_micro;
#ifdef ZX_EDITOR
		// 从编辑器启动到当前的时间（秒）
		static float curEditorTime;
#endif

		// 物理帧率
		static const int fixedFrameRate;
		// 物理帧时间（秒）
		static const float fixedDeltaTime;
		// 物理帧时间（微秒）
		static const long long fixedDeltaTime_micro;

		// 更新时间
		static void Update();
		static void UpdateCurTime();
	};
}