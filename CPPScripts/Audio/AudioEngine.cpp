#include "AudioEngine.h"
#include "AudioStream.h"
#include "AudioClip.h"
#include "../GameObject.h"

#ifdef _WIN64
#include "irrKlangImpl/AudioEngine_irrKlang.h"
#else
#include "irrKlangImpl/AudioEngine_Default.h"
#endif

namespace ZXEngine
{
	AudioEngine* AudioEngine::mInstance = nullptr;

	void AudioEngine::Create()
	{
		if (mInstance == nullptr)
		{
#ifdef _WIN64
			mInstance = new AudioEngine_irrKlang();
#else
			mInstance = new AudioEngine_Default();
#endif
		}
	}

	AudioEngine* AudioEngine::GetInstance()
	{
		return mInstance;
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
}