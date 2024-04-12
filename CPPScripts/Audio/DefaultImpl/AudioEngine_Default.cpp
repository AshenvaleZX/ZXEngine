#include "AudioEngine_Default.h"
#include "AudioClip_Default.h"
#include "AudioStream_Default.h"

namespace ZXEngine
{
	AudioClip* AudioEngine_Default::CreateAudioClip(const string& path, bool isFullPath)
	{
		return new AudioClip_Default();
	}

	AudioClip* AudioEngine_Default::CreateAudioClip(AudioStream* stream)
	{
		return new AudioClip_Default();
	}

	AudioStream* AudioEngine_Default::CreateAudioStream(const string& path, bool isFullPath)
	{
		return new AudioStream_Default();
	}
}