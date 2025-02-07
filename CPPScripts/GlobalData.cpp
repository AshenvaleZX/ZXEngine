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

	ScreenRotation GlobalData::screenRotation = ScreenRotation::Rotate0;

#ifdef ZX_PLATFORM_ANDROID
	android_app* GlobalData::app = nullptr;
#endif
}