#pragma once
#include "../../pubh.h"
#include "../AudioClip.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioEngine_irrKlang;
	class AudioStream_irrKlang;
	class AudioClip_irrKlang : public AudioClip
	{
		friend class AudioEngine;
	public:
		AudioClip_irrKlang(AudioStream_irrKlang* stream);
		virtual ~AudioClip_irrKlang();

		virtual void Play2D(bool loop = false);
		virtual void Play3D(const Vector3& position, bool loop = false);
		virtual void Stop();
		virtual void SetPause(bool pause);
		virtual void SetLoop(bool loop);
		virtual void SetVolume(float volume);
		virtual void SetPosition(const Vector3& position);
		virtual bool GetIs3D() const;
		virtual float GetLength() const;
		virtual uint32_t GetLengthMS() const;
		virtual void SetMinDistance(float distance);
		virtual float GetMinDistance() const;
		virtual void SetMaxDistance(float distance);
		virtual float GetMaxDistance() const;

	private:
		bool mIs3D = false;
		float mVolume = 1.0f;
		float mMinDistance = 1.0f;
		float mMaxDistance = 10000.0f;
		AudioEngine_irrKlang* mAudioEngine;
		AudioStream_irrKlang* mAudioStream;
		irrklang::ISound* mSound = nullptr;
	};
}