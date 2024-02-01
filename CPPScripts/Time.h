#pragma once
#include "pubh.h"
#include <chrono>

namespace ZXEngine
{
	class Time
	{
	public:
		// ��ǰϵͳʱ������룩
		static long long curSysTime;
		// ��ǰϵͳʱ�����΢�룩
		static long long curSysTime_micro;
		// ����Ϸ��������ǰ��ʱ�䣨�룩
		static float curTime;
		// ����Ϸ��������ǰ��ʱ�䣨΢�룩
		static long long curTime_micro;
		// ����һ֡�����ڵ�ʱ�䣨�룩
		static float deltaTime;
		// ����һ֡�����ڵ�ʱ�䣨΢�룩
		static long long deltaTime_micro;
#ifdef ZX_EDITOR
		// �ӱ༭����������ǰ��ʱ�䣨�룩
		static float curEditorTime;
#endif

		// ����֡��
		static const int fixedFrameRate;
		// ����֡ʱ�䣨�룩
		static const float fixedDeltaTime;
		// ����֡ʱ�䣨΢�룩
		static const long long fixedDeltaTime_micro;

		// ����ʱ��
		static void Update();
		static void UpdateCurTime();
	};
}