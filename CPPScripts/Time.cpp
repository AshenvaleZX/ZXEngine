#include "Time.h"
#ifdef ZX_EDITOR
#include "Editor/EditorDataManager.h"
#endif

namespace ZXEngine
{
	long long Time::curSysTime = 0;
	long long Time::curSysTime_micro = 0;
	long long Time::curSysStartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	long long Time::curSysStartTime_micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	float Time::curTime = 0.0f;
	long long Time::curTime_micro = 0;
	float Time::deltaTime = 0.0f;
	long long Time::deltaTime_micro = 0;
	float Time::curEditorTime = 0.0f;
	float Time::deltaTimeEditor = 0.0f;

	const int Time::fixedFrameRate = 100;
	const float Time::fixedDeltaTime = 1.0f / Time::fixedFrameRate;
	const long long Time::fixedDeltaTime_micro = 1'000'000 / Time::fixedFrameRate;

	void Time::Update()
	{
		long long time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		long long time_micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		// 如果是第一次Update，保持deltaTime为0不变，否则会变成时间戳
		if (curSysTime_micro != 0)
		{
#ifdef ZX_EDITOR
			// 编辑器时间不受暂停影响
			curEditorTime = static_cast<float>(time_micro - curSysStartTime_micro) / 1'000'000.0f;
			deltaTimeEditor = static_cast<float>(time_micro - curSysTime_micro) / 1'000'000.0f;

			if (EditorDataManager::isGameStart && EditorDataManager::isGamePause)
			{
				deltaTime = 0.02f;
				deltaTime_micro = 20'000LL;
			}
			else
			{
				deltaTime_micro = time_micro - curSysTime_micro;
				deltaTime = static_cast<float>(deltaTime_micro) / 1'000'000.0f;
			}
#else
			// 这里用微秒计算deltaTime，用毫秒有可能精度不够
			deltaTime_micro = time_micro - curSysTime_micro;
			deltaTime = static_cast<float>(deltaTime_micro) / 1'000'000.0f;
#endif
		}

#ifdef ZX_EDITOR
		// 正常运行时直接在这里更新当前游戏时间，暂停状态下需要手动点击下一帧或取消暂停才更新
		if (EditorDataManager::isGameStart && !EditorDataManager::isGamePause)
		{
			curTime += deltaTime;
			curTime_micro += deltaTime_micro;
		}
#else
		curTime += deltaTime;
		curTime_micro += deltaTime_micro;
#endif

		curSysTime = time;
		curSysTime_micro = time_micro;
	}

	void Time::UpdateCurTime()
	{
		curTime += deltaTime;
		curTime_micro += deltaTime_micro;
	}
}