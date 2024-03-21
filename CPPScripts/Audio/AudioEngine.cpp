#include "AudioEngine.h"
#include "AudioStream.h"
#include "AudioClip.h"
#include "../GameObject.h"

namespace ZXEngine
{
	AudioEngine* AudioEngine::mInstance = nullptr;

	void AudioEngine::Create()
	{
		if (mInstance == nullptr)
		{
			mInstance = new AudioEngine();
		}
	}

	AudioEngine* AudioEngine::GetInstance()
	{
		return mInstance;
	}

	AudioEngine::AudioEngine()
	{
		mEngine = irrklang::createIrrKlangDevice();

		if (!mEngine)
		{
			Debug::LogError("Could not startup the audio engine.");
		}
	}

	AudioEngine::~AudioEngine()
	{
		for (auto& iter : mAudioStreams)
			delete iter.second;

		mEngine->drop();
	}

	void AudioEngine::Update()
	{
		if (mListener)
		{
			Matrix4 mat = Math::Inverse(mListener->GetComponent<Transform>()->GetModelMatrix());

			for (auto source : mAudioSources)
			{
				if (source->GetIs3D())
				{
					Vector3 wPos = source->gameObject->GetComponent<Transform>()->GetPosition();
					Vector3 lPos = mat * Vector4(wPos, 1.0f);
					source->SetPosition(lPos);
				}
			}
		}
	}

	void AudioEngine::SetAllPause(bool pause)
	{
		for (auto& iter : mAudioClips)
		{
			iter->SetPause(pause);
		}
	}

	void AudioEngine::SetListener(GameObject* listener)
	{
		if (mListener)
		{
			Debug::LogWarning("There is already a listener in the scene.");
		}
		else
		{
			mListener = listener;
		}
	}

	void AudioEngine::RemoveListener(GameObject* listener)
	{
		if (mListener == listener)
		{
			mListener = nullptr;
		}
	}

	AudioClip* AudioEngine::CreateAudioClip(const string& path, bool isFullPath)
	{
		AudioStream* stream = CreateAudioStream(path, isFullPath);
		return new AudioClip(stream);
	}

	AudioClip* AudioEngine::CreateAudioClip(AudioStream* stream)
	{
		return new AudioClip(stream);
	}

	AudioStream* AudioEngine::CreateAudioStream(const string& path, bool isFullPath)
	{
		string fullPath = isFullPath ? path : Resources::GetAssetFullPath(path);

		auto iter = mAudioStreams.find(fullPath);
		if (iter != mAudioStreams.end())
		{
			return iter->second;
		}

		AudioStream* stream = new AudioStream(fullPath);
		mAudioStreams[fullPath] = stream;

		return stream;
	}
}