#include "ProjectSetting.h"
#include "Utils.h"
#include "Resources.h"
#include "GlobalData.h"

#ifdef ZX_EDITOR
#include "Editor/EditorGUIManager.h"
#endif

namespace ZXEngine
{
#ifdef __APPLE__
	const string ProjectSetting::OpenGLVersion = "410";
	const int ProjectSetting::OpenGLVersionMajor = 4;
	const int ProjectSetting::OpenGLVersionMinor = 1;
#else
	const string ProjectSetting::OpenGLVersion = "460";
	const int ProjectSetting::OpenGLVersionMajor = 4;
	const int ProjectSetting::OpenGLVersionMinor = 6;
#endif

	bool ProjectSetting::isSupportRayTracing = true;
	bool ProjectSetting::isSupportGeometryShader = true;
	RenderPipelineType ProjectSetting::renderPipelineType;

	string ProjectSetting::mBuiltInAssetsPath;

	uint32_t ProjectSetting::srcWidth;
	uint32_t ProjectSetting::srcHeight;
	string ProjectSetting::defaultScene;
	string ProjectSetting::projectPath;
	bool ProjectSetting::enableDynamicBatch;
	bool ProjectSetting::preserveIntermediateShader;
	bool ProjectSetting::enableGraphicsDebug;
	bool ProjectSetting::logToFile;
	bool ProjectSetting::stablePhysics;

	// Editor
	uint32_t ProjectSetting::hierarchyWidth;
	uint32_t ProjectSetting::hierarchyHeight;
	uint32_t ProjectSetting::consoleWidth;
	uint32_t ProjectSetting::consoleHeight;
	uint32_t ProjectSetting::projectWidth;
	uint32_t ProjectSetting::projectHeight;
	uint32_t ProjectSetting::inspectorWidth;
	uint32_t ProjectSetting::inspectorHeight;
	uint32_t ProjectSetting::mainBarWidth;
	uint32_t ProjectSetting::mainBarHeight;
	uint32_t ProjectSetting::gameViewWidth;
	uint32_t ProjectSetting::gameViewHeight;

	bool ProjectSetting::InitSetting(const string& path)
	{
		projectPath = path;
		Resources::SetAssetsPath(path + "/Assets/");

		string curExePath = Utils::GetCurrentExecutableFilePath();
		if (curExePath.empty())
			mBuiltInAssetsPath = "../../../BuiltInAssets/";
		else
			mBuiltInAssetsPath = curExePath + "/../../../BuiltInAssets/";

		json data = Resources::LoadJson(path + "/ProjectSetting.zxprjcfg");

		if (data == NULL)
			return false;

		GlobalData::srcWidth = data["WindowSize"][0];
		GlobalData::srcHeight = data["WindowSize"][1];
		defaultScene = Resources::JsonStrToString(data["DefaultScene"]);
		enableDynamicBatch = data["DynamicBatch"];
		preserveIntermediateShader = data["PreserveIntermediateShader"];
		enableGraphicsDebug = data["EnableGraphicsDebug"];
		logToFile = data["LogToFile"];
		stablePhysics = data["StablePhysics"];

#ifdef ZX_EDITOR
		InitWindowSize(150, 200, 300);
#else
		SetWindowSize();
#endif

		return true;
	}

#ifdef ZX_EDITOR
	void ProjectSetting::SetWindowSize()
	{
		SetWindowSize(srcWidth, srcHeight);
	}

	void ProjectSetting::SetWindowSize(uint32_t width, uint32_t height)
	{
		srcWidth = width;
		srcHeight = height;

		auto pGUIManager = EditorGUIManager::GetInstance();
		if (pGUIManager == nullptr)
		{
			gameViewWidth = GlobalData::srcWidth;
			gameViewHeight = GlobalData::srcHeight;
		}
		else
		{
			gameViewWidth = GlobalData::srcWidth + static_cast<uint32_t>(pGUIManager->mViewBorderSize.x * 2);
			gameViewHeight = GlobalData::srcHeight + static_cast<uint32_t>(pGUIManager->mViewBorderSize.y * 2 + pGUIManager->mHeaderSize);
		}

		float hRatio = static_cast<float>(hierarchyWidth) / static_cast<float>(hierarchyWidth + inspectorWidth);
		uint32_t hWidth = static_cast<uint32_t>(static_cast<float>(srcWidth - gameViewWidth) * hRatio);
		uint32_t iWidth = srcWidth - gameViewWidth - hWidth;
		uint32_t pHeight = srcHeight - gameViewHeight - ProjectSetting::mainBarHeight;

		UpdatePanelSize(hWidth, pHeight, iWidth);
	}

	void ProjectSetting::InitWindowSize(uint32_t hWidth, uint32_t pHeight, uint32_t iWidth)
	{
		gameViewWidth = GlobalData::srcWidth;
		gameViewHeight = GlobalData::srcHeight;

		hierarchyWidth = hWidth;
		hierarchyHeight = gameViewHeight;
		consoleWidth = (gameViewWidth + hierarchyWidth) / 3;
		consoleHeight = pHeight;
		projectWidth = gameViewWidth + hierarchyWidth - consoleWidth;
		projectHeight = pHeight;
		inspectorWidth = iWidth;
		inspectorHeight = gameViewHeight + projectHeight;
		mainBarWidth = gameViewWidth + hierarchyWidth + inspectorWidth;
		mainBarHeight = 58;
		srcWidth = mainBarWidth;
		srcHeight = inspectorHeight + mainBarHeight;
	}

	void ProjectSetting::UpdatePanelSize(uint32_t hWidth, uint32_t pHeight, uint32_t iWidth)
	{
		hierarchyWidth = hWidth;
		hierarchyHeight = gameViewHeight;
		consoleWidth = (gameViewWidth + hierarchyWidth) / 3;
		consoleHeight = pHeight;
		projectWidth = gameViewWidth + hierarchyWidth - consoleWidth;
		projectHeight = pHeight;
		inspectorWidth = iWidth;
		inspectorHeight = gameViewHeight + projectHeight;
		mainBarWidth = gameViewWidth + hierarchyWidth + inspectorWidth;
		mainBarHeight = 58;
	}
#else
	void ProjectSetting::SetWindowSize()
	{
		srcWidth = GlobalData::srcWidth;
		srcHeight = GlobalData::srcHeight;
	}
#endif

	void ProjectSetting::SetGameViewSize(uint32_t width, uint32_t height, EditorPanelEdgeFlags flags)
	{
		uint32_t widthDelta = width - gameViewWidth;
		uint32_t heightDelta = height - gameViewHeight;

		if (flags & ZX_EDITOR_PANEL_EDGE_LEFT)
		{
			hierarchyWidth -= widthDelta;
			consoleWidth = (width + hierarchyWidth) / 3;
			projectWidth = width + hierarchyWidth - consoleWidth;
		}
		else if (flags & ZX_EDITOR_PANEL_EDGE_RIGHT)
		{
			inspectorWidth -= widthDelta;
			consoleWidth = (srcWidth - inspectorWidth) / 3;
			projectWidth = srcWidth - inspectorWidth - consoleWidth;
		}

		if (flags & ZX_EDITOR_PANEL_EDGE_BOTTOM)
		{
			hierarchyHeight += heightDelta;
			consoleHeight -= heightDelta;
			projectHeight -= heightDelta;
		}

		if (flags & ZX_EDITOR_PANEL_EDGE_LEFT || flags & ZX_EDITOR_PANEL_EDGE_RIGHT || flags & ZX_EDITOR_PANEL_EDGE_BOTTOM)
		{
			gameViewWidth = width;
			gameViewHeight = height;
		}
	}
}