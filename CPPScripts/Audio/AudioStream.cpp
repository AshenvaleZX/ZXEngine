#include "AudioStream.h"
#include "AudioEngine.h"
#include "../Resources.h"

namespace ZXEngine
{
	AudioStream::AudioStream(const string& path)
	{
		mSoundSource = AudioEngine::GetInstance()->mEngine->addSoundSourceFromFile(path.c_str());
		mLenthMS = mSoundSource->getPlayLength();
		mLength = static_cast<float>(mLenthMS) / 1000.0f;
	}

	AudioStream::~AudioStream()
	{
		AudioEngine::GetInstance()->mEngine->removeSoundSource(mSoundSource);
	}
}