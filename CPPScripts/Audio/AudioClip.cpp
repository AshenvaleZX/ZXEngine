#include "AudioClip.h"
#include "AudioEngine.h"
#include "../Resources.h"

namespace ZXEngine
{
	AudioClip::AudioClip(const string& path)
	{
		mAudioEngine = AudioEngine::GetInstance();
		mSoundSource = mAudioEngine->mEngine->addSoundSourceFromFile(Resources::GetAssetFullPath(path).c_str());
	}

	AudioClip::~AudioClip()
	{
		if (mSound != nullptr)
			mSound->drop();

		mAudioEngine->mEngine->removeSoundSource(mSoundSource);
	}

	void AudioClip::Play2D(bool loop)
	{
		if (mSound != nullptr)
		{
			mSound->drop();
		}
		mSound = mAudioEngine->mEngine->play2D(mSoundSource, loop);
	}

	void AudioClip::Play3D(const Vector3& position, bool loop)
	{
		if (mSound != nullptr)
		{
			mSound->drop();
		}
		mSound = mAudioEngine->mEngine->play3D(mSoundSource, irrklang::vec3df(position.x, position.y, position.z), loop);
	}

	void AudioClip::Stop()
	{
		if (mSound == nullptr || mSound->isFinished())
			return;
		
		mSound->stop();
		mSound->drop();
		mSound = nullptr;
	}

	void AudioClip::SetPause(bool pause)
	{
		if (mSound == nullptr || mSound->isFinished())
			return;

		mSound->setIsPaused(pause);
	}
}