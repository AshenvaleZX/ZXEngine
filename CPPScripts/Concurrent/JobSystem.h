#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class JobHandle;
	class JobSystem
	{
	public:
		static void Create();
		static void Destroy();
		static JobSystem* GetInstance();

	private:
		static JobSystem* mInstance;

	public:
		JobSystem();
		~JobSystem();

		JobHandle AddJob(const std::function<void()>& job);

	private:
		uint32_t mCPUCoreNum;
		uint32_t mThreadNum;
		vector<std::thread> mThreads;
		queue<std::packaged_task<void()>> mJobs;

		std::mutex mJobMutex;
		std::condition_variable mJobCondition;
		std::atomic<bool> mTerminate;
	};
}