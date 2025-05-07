#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	class iOSUtil
	{
	public:
		static string GetResourcePath(const string& path, const string& ext);
        static Vector2 GetScreenSize();
	};
}
