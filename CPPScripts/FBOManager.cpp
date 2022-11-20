#include "FBOManager.h"
#include "RenderAPI.h"
#include "RenderEngine.h"

namespace ZXEngine
{
	FBOManager* FBOManager::mInstance = nullptr;

	void FBOManager::Create()
	{
		mInstance = new FBOManager();
		mInstance->mainFBO = RenderAPI::GetInstance()->CreateFrameBufferObject(FrameBufferType::Normal);
	}

	FBOManager* FBOManager::GetInstance()
	{
		return mInstance;
	}

}