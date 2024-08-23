#include "ProjectSetting.h"
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

	unsigned int ProjectSetting::srcWidth;
	unsigned int ProjectSetting::srcHeight;
	string ProjectSetting::defaultScene;
	string ProjectSetting::projectPath;
	bool ProjectSetting::enableDynamicBatch;
	bool ProjectSetting::preserveIntermediateShader;
	bool ProjectSetting::enableGraphicsDebug;
	bool ProjectSetting::logToFile;
	bool ProjectSetting::stablePhysics;

	// Editor
	unsigned int ProjectSetting::hierarchyWidth;
	unsigned int ProjectSetting::hierarchyHeight;
	unsigned int ProjectSetting::consoleWidth;
	unsigned int ProjectSetting::consoleHeight;
	unsigned int ProjectSetting::projectWidth;
	unsigned int ProjectSetting::projectHeight;
	unsigned int ProjectSetting::inspectorWidth;
	unsigned int ProjectSetting::inspectorHeight;
	unsigned int ProjectSetting::mainBarWidth;
	unsigned int ProjectSetting::mainBarHeight;
	unsigned int ProjectSetting::gameViewWidth;
	unsigned int ProjectSetting::gameViewHeight;

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

		SetWindowSize();

		return true;
	}

#ifdef ZX_EDITOR
	void ProjectSetting::SetWindowSize()
	{
		SetWindowSize(150, 200, 300);
	}

	void ProjectSetting::SetWindowSize(unsigned int width, unsigned int height)
	{
		float hRatio = static_cast<float>(hierarchyWidth) / static_cast<float>(hierarchyWidth + inspectorWidth);
		uint32_t hWidth = static_cast<uint32_t>(static_cast<float>(width - gameViewWidth) * hRatio);
		uint32_t iWidth = width - gameViewWidth - hWidth;
		uint32_t pHeight = height - gameViewHeight - ProjectSetting::mainBarHeight;

		SetWindowSize(hWidth, pHeight, iWidth);
	}

	void ProjectSetting::SetWindowSize(unsigned int hWidth, unsigned int pHeight, unsigned int iWidth)
	{
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
#else
	void ProjectSetting::SetWindowSize()
	{
		srcWidth = GlobalData::srcWidth;
		srcHeight = GlobalData::srcHeight;
	}
#endif
}