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
		static uint32_t srcWidth;
		static uint32_t srcHeight;
		static string defaultScene;
		static string projectPath;
		static bool enableDynamicBatch;
		static bool preserveIntermediateShader;
		static bool enableGraphicsDebug;
		static bool logToFile;
		static bool stablePhysics;

		// Editor
		static uint32_t hierarchyWidth;
		static uint32_t hierarchyHeight;
		static uint32_t consoleWidth;
		static uint32_t consoleHeight;
		static uint32_t projectWidth;
		static uint32_t projectHeight;
		static uint32_t inspectorWidth;
		static uint32_t inspectorHeight;
		static uint32_t mainBarWidth;
		static uint32_t mainBarHeight;
		static uint32_t gameViewWidth;
		static uint32_t gameViewHeight;

		static bool InitSetting(const string& path);

#ifdef ZX_EDITOR
		static void SetWindowSize();
		static void SetWindowSize(uint32_t width, uint32_t height);
#else
		static void SetWindowSize();
#endif
		static void SetGameViewSize(uint32_t width, uint32_t height, EditorPanelEdgeFlags flags);

#ifdef ZX_EDITOR
	private:
		static void InitWindowSize(uint32_t hWidth, uint32_t pHeight, uint32_t iWidth);
		static void UpdatePanelSize(uint32_t hWidth, uint32_t pHeight, uint32_t iWidth);
#endif
	};
}