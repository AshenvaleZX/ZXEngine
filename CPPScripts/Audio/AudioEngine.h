#pragma once
#include "../pubh.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioEngine 
	{
		friend class AudioClip;
		friend class AudioStream;
	public:
		static void Create();
		static AudioEngine* GetInstance();

	private:
		static AudioEngine* mInstance;

	public:
		AudioEngine();
		~AudioEngine();

		void SetAllPause(bool pause);

		AudioClip* CreateAudioClip(const string& path);
		AudioClip* CreateAudioClip(AudioStream* stream);
		AudioStream* CreateAudioStream(const string& path);

	private:
		irrklang::ISoundEngine* mEngine;
		vector<AudioClip*> mAudioClips;
		unordered_map<string, AudioStream*> mAudioStreams;
	};
}