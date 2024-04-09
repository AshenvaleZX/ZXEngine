#pragma once
#include "../../pubh.h"
#include "../AudioEngine.h"

namespace ZXEngine
{
	class AudioClip;
	class AudioStream;
	class AudioEngine_Default : public AudioEngine
	{
	public:
		AudioEngine_Default() {};
		~AudioEngine_Default() {};

		virtual AudioClip* CreateAudioClip(const string& path, bool isFullPath = false);
		virtual AudioClip* CreateAudioClip(AudioStream* stream);
		virtual AudioStream* CreateAudioStream(const string& path, bool isFullPath = false);
	};
}