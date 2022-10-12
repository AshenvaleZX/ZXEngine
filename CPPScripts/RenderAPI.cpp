#include "RenderAPI.h"
#include "RenderAPIOpenGL.h"

namespace ZXEngine
{
	RenderAPI* RenderAPI::Creat()
	{
		RenderAPI* api = nullptr;
		api = new RenderAPIOpenGL();
		return api;
	}
}