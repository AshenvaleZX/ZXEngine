#include "Time.h"
#ifdef ZX_EDITOR
#include "Editor/EditorDataManager.h"
#endif

namespace ZXEngine
{
	long long Time::curSysTime = 0;
	long long Time::curSysTime_micro = 0;
	float Time::curTime = 0.0f;
	float Time::deltaTime = 0.0f;

	void Time::Update()
	{
		long long time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		long long time_micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		// 如果是第一次Update，保持deltaTime为0不变，否则会变成时间戳
		if (curSysTime_micro != 0)
		{
#ifdef ZX_EDITOR
			if (EditorDataManager::isGameStart && EditorDataManager::isGamePause)
				deltaTime = 0.02f;
			else
				deltaTime = (time_micro - curSysTime_micro) / 1000000.0f;
#else
			// 这里用微秒计算deltaTime，用毫秒有可能精度不够
			deltaTime = (time_micro - curSysTime_micro) / 1000000.0f;
#endif
		}

#ifdef ZX_EDITOR
		// 正常运行时直接在这里更新当前游戏时间，暂停状态下需要手动点击下一帧或取消暂停才更新
		if (EditorDataManager::isGameStart && !EditorDataManager::isGamePause)
			curTime += deltaTime;
#else
		curTime += deltaTime;
#endif

		curSysTime = time;
		curSysTime_micro = time_micro;
	}

	void Time::UpdateCurTime()
	{
		curTime += deltaTime;
	}
}