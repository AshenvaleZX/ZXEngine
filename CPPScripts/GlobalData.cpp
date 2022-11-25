#include "GlobalData.h"

namespace ZXEngine
{
	unsigned int GlobalData::srcWidth = 0;
	unsigned int GlobalData::srcHeight = 0;
	float GlobalData::shadowCubeMapNearPlane = 1.0f;
	float GlobalData::shadowCubeMapFarPlane = 100.0f;
	unsigned int GlobalData::depthCubeMapWidth = 1024;
}