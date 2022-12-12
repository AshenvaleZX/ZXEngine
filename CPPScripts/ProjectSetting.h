#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class ProjectSetting
	{
	public:
		static unsigned int srcWidth;
		static unsigned int srcHeight;
		static string defaultScene;
		static string projectPath;

		// Editor
		static unsigned int hierarchyWidth;
		static unsigned int hierarchyHeight;
		static unsigned int fileWidth;
		static unsigned int fileHeight;
		static unsigned int inspectorWidth;
		static unsigned int inspectorHeight;
		static unsigned int mainBarWidth;
		static unsigned int mainBarHeight;

		static void InitSetting(string path);

#ifdef ZX_EDITOR
		static void SetWindowSize(unsigned int hWidth, unsigned int fHeight, unsigned int iWidth);
#else
		static void SetWindowSize();
#endif
	};
}