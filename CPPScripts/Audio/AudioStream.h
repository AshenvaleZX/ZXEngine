#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	// ��Ƶ������������ֻ����ͨ��AudioEngine����������
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