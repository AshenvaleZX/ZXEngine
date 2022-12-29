#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class CubeMap
	{
	public:
		CubeMap(vector<string> path);
		~CubeMap();

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}