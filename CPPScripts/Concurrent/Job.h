#pragma once
#include "../pubh.h"

namespace ZXEngine
{
    class JobHandle
    {
    public:
        JobHandle(std::future<void>&& future) : 
            mFuture(std::move(future))
        {}

        void Accomplish()
        {
            mFuture.get();
        }

    private:
        std::future<void> mFuture;
    };

	class Job
	{
    public:
        JobHandle Schedule();
        virtual void Execute() = 0;
	};
}