#pragma once
#include <chrono>

namespace ZXEngine
{
	class Time
	{
	public:
		// ��ǰϵͳʱ������룩
		static long long curTime;
		// ��ǰϵͳʱ��������룩
		static long long curTime_milli;
		// ��ǰϵͳʱ�����΢�룩
		static long long curTime_micro;
		static float deltaTime;
		// ����ʱ��
		static void Update();
	};
}