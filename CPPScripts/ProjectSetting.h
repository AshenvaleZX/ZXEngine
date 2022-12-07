#pragma once
#include <string>

namespace ZXEngine
{
	class ProjectSetting
	{
	public:
		static unsigned int srcWidth;
		static unsigned int srcHeight;
		static std::string defaultScene;
		static std::string projectPath;

		static void InitSetting(std::string path);
	};
}