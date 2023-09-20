#include "GlobalData.h"

namespace ZXEngine
{
	uint32_t GlobalData::srcWidth = 0;
	uint32_t GlobalData::srcHeight = 0;

	uint32_t GlobalData::depthMapWidth = 1024;
	float GlobalData::shadowMapFarPlane = 100.0f;
	float GlobalData::shadowMapNearPlane = 1.0f;

	uint32_t GlobalData::depthCubeMapWidth = 1024;
	float GlobalData::shadowCubeMapFarPlane = 100.0f;
	float GlobalData::shadowCubeMapNearPlane = 1.0f;
}