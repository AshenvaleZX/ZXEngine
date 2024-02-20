#pragma once
#include "../pubh.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioEngine;
	class AudioClip
	{
		friend class AudioEngine;
	public:
		AudioClip(const string& path);
		~AudioClip();

		void Play2D(bool loop = false);
		void Play3D(const Vector3& position, bool loop = false);
		void Stop();
		void SetPause(bool pause);

	private:
		AudioEngine* mAudioEngine;
		irrklang::ISound* mSound = nullptr;
		irrklang::ISoundSource* mSoundSource;
	};
}