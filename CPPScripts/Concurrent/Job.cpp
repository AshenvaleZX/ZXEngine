#include "Job.h"
#include "JobSystem.h"

namespace ZXEngine
{
	JobHandle Job::Schedule()
	{
		std::function<void()> func = std::bind(&Job::Execute, this);
		return JobSystem::GetInstance()->AddJob(func);
	}
}