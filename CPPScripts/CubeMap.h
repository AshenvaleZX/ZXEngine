#pragma once
#include "RenderAPI.h"

namespace ZXEngine
{
	class CubeMap
	{
	public:
		CubeMap(const vector<string>& path);
		~CubeMap();

		unsigned int GetID();

	private:
		unsigned int ID;
	};
}