#include "AudioEngine.h"

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
		mEngine->drop();
	}
}