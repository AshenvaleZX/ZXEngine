#pragma once
#include "../pubh.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioEngine;
	class AudioStream;
	class AudioClip
	{
		friend class AudioEngine;
	public:
		AudioClip(AudioStream* stream);
		~AudioClip();

		void Play2D(bool loop = false);
		void Play3D(const Vector3& position, bool loop = false);
		void Stop();
		void SetPause(bool pause);

	private:
		AudioEngine* mAudioEngine;
		AudioStream* mAudioStream;
		irrklang::ISound* mSound = nullptr;
	};
}