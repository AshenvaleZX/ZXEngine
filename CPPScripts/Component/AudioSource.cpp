#include "AudioSource.h"
#include "../Audio/ZAudio.h"
#include "../GameObject.h"
#include "../Resources.h"

#ifdef ZX_EDITOR
#include "../Editor/EditorDataManager.h"
#endif

namespace ZXEngine
{
	ComponentType AudioSource::GetType()
	{
		return ComponentType::AudioSource;
	}

	ComponentType AudioSource::GetInsType()
	{
		return ComponentType::AudioSource;
	}

	AudioSource::~AudioSource()
	{
		if (mAudioClip != nullptr)
			delete mAudioClip;
	}

	void AudioSource::Awake()
	{
		if (mPlayOnAwake)
		{
			if (mIs3D)
				Play3D(mIsLoop);
			else
				Play2D(mIsLoop);
		}

#ifdef ZX_EDITOR
		// 在编辑器模式下，如果游戏以暂停状态启动，音频也应该处于暂停状态
		if (EditorDataManager::isGamePause)
		{
			SetPause(true);
		}
#endif
		mIsAwake = true;
	}

	void AudioSource::Init(const string& path)
	{
		mName = Resources::GetAssetNameWithExtension(path);
		mAudioClip = AudioEngine::GetInstance()->CreateAudioClip(path);
	}

	void AudioSource::Play2D(bool loop)
	{
		mIs3D = false;
		mIsLoop = loop;
		mAudioClip->Play2D(loop);
	}

	void AudioSource::Play3D(bool loop)
	{
		mIs3D = true;
		mIsLoop = loop;

		Vector3 position = gameObject->GetComponent<Transform>()->GetPosition();
		mAudioClip->Play3D(position, loop);
	}

	void AudioSource::Play3D(const Vector3& position, bool loop)
	{
		mIs3D = true;
		mIsLoop = loop;
		mAudioClip->Play3D(position, loop);
	}

	bool AudioSource::GetIs3D()
	{
		return mIs3D;
	}

	void AudioSource::Stop()
	{
		mAudioClip->Stop();
	}

	void AudioSource::SetPause(bool pause)
	{
		mIsPause = pause;
		mAudioClip->SetPause(pause);
	}

	bool AudioSource::GetPause()
	{
		return mIsPause;
	}

	void AudioSource::SetLoop(bool loop)
	{
		mIsLoop = loop;
		mAudioClip->SetLoop(loop);
	}

	bool AudioSource::GetLoop()
	{
		return mIsLoop;
	}

	void AudioSource::SetVolume(float volume)
	{
		mVolume = volume;
		mAudioClip->SetVolume(volume);
	}

	float AudioSource::GetVolume()
	{
		return mVolume;
	}

	void AudioSource::SetPosition(const Vector3& position)
	{
		mAudioClip->SetPosition(position);
	}
}