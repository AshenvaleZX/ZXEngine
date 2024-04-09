#include "AudioClip_irrKlang.h"
#include "AudioEngine_irrKlang.h"
#include "AudioStream_irrKlang.h"
#include "../../Resources.h"

namespace ZXEngine
{
	AudioClip_irrKlang::AudioClip_irrKlang(AudioStream_irrKlang* stream)
	{
		mAudioStream = stream;
		mAudioEngine = AudioEngine_irrKlang::GetInstance();
		mAudioEngine->mAudioClips.push_back(this);
	}

	AudioClip_irrKlang::~AudioClip_irrKlang()
	{
		if (mSound != nullptr)
		{
			mSound->stop();
			mSound->drop();
		}
		mAudioEngine->mAudioClips.erase(std::remove(mAudioEngine->mAudioClips.begin(), mAudioEngine->mAudioClips.end(), this), mAudioEngine->mAudioClips.end());
	}

	void AudioClip_irrKlang::Play2D(bool loop)
	{
		if (mSound != nullptr)
		{
			mSound->stop();
			mSound->drop();
		}
		mSound = mAudioEngine->mEngine->play2D(mAudioStream->mSoundSource, loop, false, true);
		mSound->setVolume(mVolume);
		mIs3D = false;
	}

	void AudioClip_irrKlang::Play3D(const Vector3& position, bool loop)
	{
		if (mSound != nullptr)
		{
			mSound->stop();
			mSound->drop();
		}
		mSound = mAudioEngine->mEngine->play3D(mAudioStream->mSoundSource, irrklang::vec3df(position.x, position.y, position.z), loop, false, true);
		mSound->setVolume(mVolume);
		mSound->setMinDistance(mMinDistance);
		mSound->setMaxDistance(mMaxDistance);
		mIs3D = true;
	}

	void AudioClip_irrKlang::Stop()
	{
		if (mSound == nullptr || mSound->isFinished())
			return;

		mSound->stop();
		mSound->drop();
		mSound = nullptr;
	}

	void AudioClip_irrKlang::SetPause(bool pause)
	{
		if (mSound == nullptr || mSound->isFinished())
			return;

		mSound->setIsPaused(pause);
	}

	void AudioClip_irrKlang::SetLoop(bool loop)
	{
		if (mSound == nullptr || mSound->isFinished())
			return;

		mSound->setIsLooped(loop);
	}

	void AudioClip_irrKlang::SetVolume(float volume)
	{
		mVolume = volume;

		if (mSound == nullptr || mSound->isFinished())
			return;

		mSound->setVolume(volume);
	}

	void AudioClip_irrKlang::SetPosition(const Vector3& position)
	{
		if (mSound == nullptr || mSound->isFinished())
			return;

		mSound->setPosition(irrklang::vec3df(position.x, position.y, position.z));
	}

	bool AudioClip_irrKlang::GetIs3D() const
	{
		return mIs3D;
	}

	float AudioClip_irrKlang::GetLength() const
	{
		return mAudioStream->mLength;
	}

	uint32_t AudioClip_irrKlang::GetLengthMS() const
	{
		return mAudioStream->mLenthMS;
	}

	void AudioClip_irrKlang::SetMinDistance(float distance)
	{
		mMinDistance = distance;

		if (mSound)
			mSound->setMinDistance(distance);
	}

	float AudioClip_irrKlang::GetMinDistance() const
	{
		return mMinDistance;
	}

	void AudioClip_irrKlang::SetMaxDistance(float distance)
	{
		mMaxDistance = distance;

		if (mSound)
			mSound->setMaxDistance(distance);
	}

	float AudioClip_irrKlang::GetMaxDistance() const
	{
		return mMaxDistance;
	}
}