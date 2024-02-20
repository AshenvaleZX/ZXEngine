#pragma once
#include "../pubh.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	// ��Ƶ������������ֻ����ͨ��AudioEngine����������
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