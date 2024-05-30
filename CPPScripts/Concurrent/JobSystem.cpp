#include "JobSystem.h"
#include "Job.h"

#if defined(_WIN64) && defined(_DEBUG)
#include <windows.h>
#undef AddJob
#endif

namespace ZXEngine
{
	JobSystem* JobSystem::mInstance = nullptr;

	void JobSystem::Create()
	{
		mInstance = new JobSystem();
	}

	void JobSystem::Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}

	JobSystem* JobSystem::GetInstance()
	{
		return mInstance;
	}

	JobSystem::JobSystem()
	{
		mCPUCoreNum = static_cast<uint32_t>(std::thread::hardware_concurrency());

		if (mCPUCoreNum > 1)
			mThreadNum = mCPUCoreNum - 1;
		else
			mThreadNum = 1;

		mThreads.reserve(mThreadNum);
		mTerminate = false;

		for (uint32_t i = 0; i < mThreadNum; i++)
		{
			mThreads.emplace_back([this]
			{
				while (true)
				{
					std::packaged_task<void()> job;

					{
						std::unique_lock<std::mutex> lock(this->mJobMutex);

						this->mJobCondition.wait(lock, [this] 
						{
							return this->mTerminate || !this->mJobs.empty();
						});

						if (this->mTerminate && this->mJobs.empty())
							return;

						job = std::move(this->mJobs.front());

						this->mJobs.pop();
					}

					job();
				}
			});
#if defined(_WIN64) && defined(_DEBUG)
			SetThreadDescription(mThreads[i].native_handle(), L"JobSystemThread");
#endif
		}
	}

	JobSystem::~JobSystem()
	{
		mTerminate = true;

		mJobCondition.notify_all();

		for (std::thread& thread : mThreads)
			thread.join();
	}

	JobHandle JobSystem::AddJob(const std::function<void()>& job)
	{
		std::packaged_task<void()> packagedJob(job);
		std::future<void> future = packagedJob.get_future();

		{
			std::unique_lock<std::mutex> lock(mJobMutex);
			mJobs.push(std::move(packagedJob));
		}

		mJobCondition.notify_one();

		return JobHandle(std::move(future));
	}
}