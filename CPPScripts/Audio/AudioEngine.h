#pragma once
#include "../pubh.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioEngine 
	{
		friend class AudioClip;
	public:
		static void Create();
		static AudioEngine* GetInstance();

	private:
		static AudioEngine* mInstance;

	public:
		AudioEngine();
		~AudioEngine();

	private:
		irrklang::ISoundEngine* mEngine;
	};
}