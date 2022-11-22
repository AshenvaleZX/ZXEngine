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
		mInstance->shadowCubeMapFBO = RenderAPI::GetInstance()->CreateFrameBufferObject(FrameBufferType::ShadowCubeMap);
	}

	FBOManager* FBOManager::GetInstance()
	{
		return mInstance;
	}

}