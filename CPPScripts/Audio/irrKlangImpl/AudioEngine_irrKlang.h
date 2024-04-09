#pragma once
#include "../../pubh.h"
#include "../AudioEngine.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioClip;
	class AudioStream;
	class AudioStream_irrKlang;
	class AudioEngine_irrKlang : public AudioEngine
	{
		friend class AudioClip_irrKlang;
		friend class AudioStream_irrKlang;
		friend class AudioSource_irrKlang;
	public:
		static AudioEngine_irrKlang* GetInstance();

	private:
		static AudioEngine_irrKlang* mInstance;

	public:
		AudioEngine_irrKlang();
		~AudioEngine_irrKlang();

		virtual AudioClip* CreateAudioClip(const string& path, bool isFullPath = false);
		virtual AudioClip* CreateAudioClip(AudioStream* stream);
		virtual AudioStream* CreateAudioStream(const string& path, bool isFullPath = false);

	private:
		irrklang::ISoundEngine* mEngine;
		unordered_map<string, AudioStream_irrKlang*> mAudioStreams;
	};
}