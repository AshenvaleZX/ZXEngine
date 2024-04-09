#include "AudioEngine_irrKlang.h"
#include "AudioStream_irrKlang.h"
#include "AudioClip_irrKlang.h"
#include "../../Resources.h"

namespace ZXEngine
{
	AudioEngine_irrKlang* AudioEngine_irrKlang::mInstance = nullptr;

	AudioEngine_irrKlang* AudioEngine_irrKlang::GetInstance()
	{
		return mInstance;
	}

	AudioEngine_irrKlang::AudioEngine_irrKlang()
	{
		mEngine = irrklang::createIrrKlangDevice();

		if (!mEngine)
		{
			Debug::LogError("Could not startup the audio engine.");
		}

		mInstance = this;
	}

	AudioEngine_irrKlang::~AudioEngine_irrKlang()
	{
		for (auto& iter : mAudioStreams)
			delete iter.second;

		mEngine->drop();
	}

	AudioClip* AudioEngine_irrKlang::CreateAudioClip(const string& path, bool isFullPath)
	{
		AudioStream* stream = CreateAudioStream(path, isFullPath);
		return new AudioClip_irrKlang(dynamic_cast<AudioStream_irrKlang*>(stream));
	}

	AudioClip* AudioEngine_irrKlang::CreateAudioClip(AudioStream* stream)
	{
		return new AudioClip_irrKlang(dynamic_cast<AudioStream_irrKlang*>(stream));
	}

	AudioStream* AudioEngine_irrKlang::CreateAudioStream(const string& path, bool isFullPath)
	{
		string fullPath = isFullPath ? path : Resources::GetAssetFullPath(path);

		auto iter = mAudioStreams.find(fullPath);
		if (iter != mAudioStreams.end())
		{
			return iter->second;
		}

		AudioStream_irrKlang* stream = new AudioStream_irrKlang(fullPath);
		mAudioStreams[fullPath] = stream;

		return stream;
	}
}