#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class GlobalData
	{
	public:
		static uint32_t srcWidth;
		static uint32_t srcHeight;

		static uint32_t depthMapWidth;
		static float shadowMapFarPlane;
		static float shadowMapNearPlane;

		static uint32_t depthCubeMapWidth;
		static float shadowCubeMapFarPlane;
		static float shadowCubeMapNearPlane;
	};
}