#pragma once
#include <mutex>

template<typename T>
class ThreadSafeData
{
public:
	ThreadSafeData() = default;
	ThreadSafeData(const T& data) : mData(data) {}

	inline T Get()
	{
		std::lock_guard<std::mutex> lock(mMutex);
		return mData;
	}

	inline void Dump(T& data)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		data = std::move(mData);
	}

	inline void Set(const T& data)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mData = data;
	}

	inline void Move(const T& data)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		mData = std::move(data);
	}

private:
	T mData;
	std::mutex mMutex;
};