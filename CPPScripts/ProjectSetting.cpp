#include "ProjectSetting.h"
#include "Resources.h"
#include "GlobalData.h"

namespace ZXEngine
{
	unsigned int ProjectSetting::srcWidth;
	unsigned int ProjectSetting::srcHeight;
	string ProjectSetting::defaultScene;
	string ProjectSetting::projectPath;

	// Editor
	unsigned int ProjectSetting::hierarchyWidth;
	unsigned int ProjectSetting::hierarchyHeight;
	unsigned int ProjectSetting::projectWidth;
	unsigned int ProjectSetting::projectHeight;
	unsigned int ProjectSetting::inspectorWidth;
	unsigned int ProjectSetting::inspectorHeight;
	unsigned int ProjectSetting::mainBarWidth;
	unsigned int ProjectSetting::mainBarHeight;

	void ProjectSetting::InitSetting(string path)
	{
		projectPath = path;
		Resources::SetAssetsPath(path + "/Assets/");

		json data = Resources::LoadJson(path + "/ProjectSetting.zxprjcfg");

		GlobalData::srcWidth = data["WindowSize"][0];
		GlobalData::srcHeight = data["WindowSize"][1];
		defaultScene = Resources::JsonStrToString(data["DefaultScene"]);

#ifdef ZX_EDITOR
		SetWindowSize(200, 200, 200);
#else
		SetWindowSize();
#endif
	}

#ifdef ZX_EDITOR
	void ProjectSetting::SetWindowSize(unsigned int hWidth, unsigned int pHeight, unsigned int iWidth)
	{
		hierarchyWidth = hWidth;
		hierarchyHeight = GlobalData::srcHeight;
		projectWidth = GlobalData::srcWidth + hierarchyWidth;
		projectHeight = pHeight;
		inspectorWidth = iWidth;
		inspectorHeight = GlobalData::srcHeight + projectHeight;
		mainBarWidth = GlobalData::srcWidth + hierarchyWidth + inspectorWidth;
		mainBarHeight = 50;
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