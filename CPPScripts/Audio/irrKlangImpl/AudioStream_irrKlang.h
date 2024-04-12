#pragma once
#include "../../pubh.h"
#include "../AudioStream.h"
#include <irrKlang/irrKlang.h>

namespace ZXEngine
{
	class AudioStream_irrKlang : public AudioStream
	{
	public:
		irrklang::ISoundSource* mSoundSource;

		AudioStream_irrKlang(const string& path);
		virtual ~AudioStream_irrKlang();
	};
}