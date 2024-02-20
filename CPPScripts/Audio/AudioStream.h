#pragma once
#include "../pubh.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	// 音频数据流，此类只允许通过AudioEngine创建和销毁
	class AudioStream
	{
		friend class AudioClip;
		friend class AudioEngine;
	public:
		float mLength;
		uint32_t mLenthMS;

		AudioStream() = delete;

	private:
		AudioStream(const string& path);
		~AudioStream();

		irrklang::ISoundSource* mSoundSource;
	};
}