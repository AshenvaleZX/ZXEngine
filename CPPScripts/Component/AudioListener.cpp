#include "AudioListener.h"
#include "../Audio/AudioEngine.h"

namespace ZXEngine
{
	ComponentType AudioListener::GetType()
	{
		return ComponentType::AudioListener;
	}

	ComponentType AudioListener::GetInsType()
	{
		return ComponentType::AudioListener;
	}

	AudioListener::~AudioListener()
	{
		AudioEngine::GetInstance()->RemoveListener(gameObject);
	}

	void AudioListener::Awake()
	{
		AudioEngine::GetInstance()->SetListener(gameObject);
	}
}