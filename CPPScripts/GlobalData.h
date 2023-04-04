#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class GlobalData
	{
	public:
		static unsigned int srcWidth;
		static unsigned int srcHeight;

		static float shadowCubeMapNearPlane;
		static float shadowCubeMapFarPlane;
		static unsigned int depthCubeMapWidth;
	};
}