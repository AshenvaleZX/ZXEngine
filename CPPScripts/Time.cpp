#include "Time.h"

namespace ZXEngine
{
	long long Time::curTime = 0;
	long long Time::curTime_milli = 0;
	long long Time::curTime_micro = 0;
	float Time::deltaTime = 0;

	void Time::Update()
	{
		long long time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		long long time_milli = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		long long time_micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		// 如果是第一次Update，保持deltaTime为0不变，否则会变成时间戳
		if (curTime_micro != 0)
		{
			// 这里用微秒计算deltaTime，用毫秒有可能精度不够
			deltaTime = (time_micro - curTime_micro) / 100000.0f;
		}

		curTime = time;
		curTime_milli = time_milli;
		curTime_micro = time_micro;
	}
}