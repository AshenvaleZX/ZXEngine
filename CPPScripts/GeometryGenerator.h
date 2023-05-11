#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class StaticMesh;
	class GeometryGenerator
	{
	public:
		static StaticMesh* CreateBox(float xLength, float yLength, float zLength);
		static StaticMesh* CreateSphere(float radius, uint32_t sliceCount, uint32_t stackCount);
	};

}