#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	// 音频数据流，此类只允许通过AudioEngine创建和销毁
	class AudioStream
	{
	protected:
		AudioStream() {};
		virtual ~AudioStream() {};

	public:
		float mLength = 0.0f;
		uint32_t mLenthMS = 0;
	};
}