#include "AudioStream_irrKlang.h"
#include "AudioEngine_irrKlang.h"
#include "../../Resources.h"

namespace ZXEngine
{
	AudioStream_irrKlang::AudioStream_irrKlang(const string& path)
	{
		mSoundSource = AudioEngine_irrKlang::GetInstance()->mEngine->addSoundSourceFromFile(path.c_str());
		mLenthMS = mSoundSource->getPlayLength();
		mLength = static_cast<float>(mLenthMS) / 1000.0f;
	}

	AudioStream_irrKlang::~AudioStream_irrKlang()
	{
		AudioEngine_irrKlang::GetInstance()->mEngine->removeSoundSource(mSoundSource);
	}
}