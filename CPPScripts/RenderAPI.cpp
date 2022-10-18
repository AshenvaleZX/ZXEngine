#include "RenderAPI.h"
#include "RenderAPIOpenGL.h"

namespace ZXEngine
{
	RenderAPI* RenderAPI::mInstance = nullptr;

	void RenderAPI::Creat()
	{
		mInstance = new RenderAPIOpenGL();
	}

	RenderAPI* RenderAPI::GetInstance()
	{
		return mInstance;
	}
}