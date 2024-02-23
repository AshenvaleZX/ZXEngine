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
		void SetLoop(bool loop);
		void SetVolume(float volume);
		void SetPosition(const Vector3& position);
		bool GetIs3D();
		float GetLength();
		uint32_t GetLengthMS();
		void SetMinDistance(float distance);
		float GetMinDistance();
		void SetMaxDistance(float distance);
		float GetMaxDistance();

	private:
		bool mIs3D = false;
		float mVolume = 1.0f;
		float mMinDistance = 1.0f;
		float mMaxDistance = 10000.0f;
		AudioEngine* mAudioEngine;
		AudioStream* mAudioStream;
		irrklang::ISound* mSound = nullptr;
	};
}