#pragma once
#include "../../pubh.h"
#include "../AudioClip.h"

namespace ZXEngine
{
	class AudioClip_Default : public AudioClip
	{
	public:
		AudioClip_Default() {};
		virtual ~AudioClip_Default() {};

		virtual void Play2D(bool loop = false) {};
		virtual void Play3D(const Vector3& position, bool loop = false) {};
		virtual void Stop() {};
		virtual void SetPause(bool pause) {};
		virtual void SetLoop(bool loop) {};
		virtual void SetVolume(float volume) {};
		virtual void SetPosition(const Vector3& position) {};
		virtual bool GetIs3D() const { return false; };
		virtual float GetLength() const { return 0.0f; };
		virtual uint32_t GetLengthMS() const { return 0; };
		virtual void SetMinDistance(float distance) {};
		virtual float GetMinDistance() const { return 0.0f; };
		virtual void SetMaxDistance(float distance) {};
		virtual float GetMaxDistance() const { return 0.0f; };
	};
}