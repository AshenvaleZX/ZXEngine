#include "RenderAPI.h"
#include "RenderAPIOpenGL.h"

namespace ZXEngine
{
	RenderAPI* RenderAPI::mInstance = nullptr;

	void RenderAPI::Creat()
	{
		mInstance = new RenderAPIOpenGL();
	}

	RenderAPI* RenderAPI::Get()
	{
		return mInstance;
	}
}