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
		// ����һ֡�����ڵ�ʱ��
		static float deltaTime;
#ifdef ZX_EDITOR
		// �ӱ༭����������ǰ��ʱ�䣨�룩
		static float curEditorTime;
#endif

		// ����ʱ��
		static void Update();
		static void UpdateCurTime();
	};
}