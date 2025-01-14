#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class AudioClip
	{
		// 不允许外部直接创建此基类，需要通过AudioEngine管理创建
	protected:
		AudioClip() {};

	public:
		virtual ~AudioClip() {};

		virtual void Play2D(bool loop = false) = 0;
		virtual void Play3D(const Vector3& position, bool loop = false) = 0;
		virtual void Stop() = 0;
		virtual void SetPause(bool pause) = 0;
		virtual void SetLoop(bool loop) = 0;
		virtual void SetVolume(float volume) = 0;
		virtual void SetPosition(const Vector3& position) = 0;
		virtual bool GetIs3D() const = 0;
		virtual float GetLength() const = 0;
		virtual uint32_t GetLengthMS() const = 0;
		virtual void SetMinDistance(float distance) = 0;
		virtual float GetMinDistance() const = 0;
		virtual void SetMaxDistance(float distance) = 0;
		virtual float GetMaxDistance() const = 0;
	};
}