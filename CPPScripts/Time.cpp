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

		// ����ǵ�һ��Update������deltaTimeΪ0���䣬�������ʱ���
		if (curSysTime_micro != 0)
		{
#ifdef ZX_EDITOR
			if (EditorDataManager::isGameStart && EditorDataManager::isGamePause)
				deltaTime = 0.02f;
			else
				deltaTime = (time_micro - curSysTime_micro) / 1000000.0f;
#else
			// ������΢�����deltaTime���ú����п��ܾ��Ȳ���
			deltaTime = (time_micro - curSysTime_micro) / 1000000.0f;
#endif
		}

#ifdef ZX_EDITOR
		// ��������ʱֱ����������µ�ǰ��Ϸʱ�䣬��ͣ״̬����Ҫ�ֶ������һ֡��ȡ����ͣ�Ÿ���
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