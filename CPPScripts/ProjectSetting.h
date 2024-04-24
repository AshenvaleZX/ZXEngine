#pragma once
#include "pubh.h"

namespace ZXEngine
{
	class ProjectSetting
	{
	public:
		static const string OpenGLVersion;
		static const int OpenGLVersionMajor;
		static const int OpenGLVersionMinor;

		static bool isSupportRayTracing;
		static bool isSupportGeometryShader;
		static RenderPipelineType renderPipelineType;

		static string mBuiltInAssetsPath;

		// ProjectSetting.zxprjcfg
		static unsigned int srcWidth;
		static unsigned int srcHeight;
		static string defaultScene;
		static string projectPath;
		static bool enableDynamicBatch;
		static bool preserveIntermediateShader;
		static bool enableGraphicsDebug;
		static bool logToFile;
		static bool stablePhysics;

		// Editor
		static unsigned int hierarchyWidth;
		static unsigned int hierarchyHeight;
		static unsigned int consoleWidth;
		static unsigned int consoleHeight;
		static unsigned int projectWidth;
		static unsigned int projectHeight;
		static unsigned int inspectorWidth;
		static unsigned int inspectorHeight;
		static unsigned int mainBarWidth;
		static unsigned int mainBarHeight;

		static bool InitSetting(const string& path);

#ifdef ZX_EDITOR
		static void SetWindowSize(unsigned int hWidth, unsigned int pHeight, unsigned int iWidth);
#else
		static void SetWindowSize();
#endif
	};
}