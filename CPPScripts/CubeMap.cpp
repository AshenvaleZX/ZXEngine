#include "CubeMap.h"

namespace ZXEngine
{
	CubeMap::CubeMap(vector<string> path)
	{
		ID = RenderAPI::GetInstance()->LoadCubeMap(path);
	}

	unsigned int CubeMap::GetID()
	{
		return ID;
	}
}