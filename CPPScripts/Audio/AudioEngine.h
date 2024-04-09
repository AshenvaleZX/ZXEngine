#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class GameObject;
	class AudioClip;
	class AudioStream;
	class AudioSource;
	class AudioEngine 
	{
		friend class AudioSource;
	public:
		static void Create();
		static AudioEngine* GetInstance();

	private:
		static AudioEngine* mInstance;

	public:
		AudioEngine() {};
		~AudioEngine() {};

		void Update();
		void SetAllPause(bool pause);
		void SetListener(GameObject* listener);
		void RemoveListener(GameObject* listener);

		virtual AudioClip* CreateAudioClip(const string& path, bool isFullPath = false) = 0;
		virtual AudioClip* CreateAudioClip(AudioStream* stream) = 0;
		virtual AudioStream* CreateAudioStream(const string& path, bool isFullPath = false) = 0;

	protected:
		GameObject* mListener = nullptr;
		vector<AudioClip*> mAudioClips;
		vector<AudioSource*> mAudioSources;
	};
}