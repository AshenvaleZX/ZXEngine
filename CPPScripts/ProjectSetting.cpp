#include "ProjectSetting.h"
#include "Resources.h"
#include "GlobalData.h"

namespace ZXEngine
{
	unsigned int ProjectSetting::srcWidth;
	unsigned int ProjectSetting::srcHeight;
	std::string ProjectSetting::defaultScene;
	std::string ProjectSetting::projectPath;

	void ProjectSetting::InitSetting(std::string path)
	{
		ProjectSetting::projectPath = path;
		Resources::SetAssetsPath(path + "/Assets/");

		json data = Resources::LoadJson(path + "/ProjectSetting.zxprjcfg");

#ifdef ZX_EDITOR
		GlobalData::srcWidth = data["WindowSize"][0];
		GlobalData::srcHeight = data["WindowSize"][1];
		ProjectSetting::srcWidth = GlobalData::srcWidth + 100;
		ProjectSetting::srcHeight = GlobalData::srcHeight + 100;
#else
		GlobalData::srcWidth = data["WindowSize"][0];
		GlobalData::srcHeight = data["WindowSize"][1];
		ProjectSetting::srcWidth = GlobalData::srcWidth;
		ProjectSetting::srcHeight = GlobalData::srcHeight;
#endif
		ProjectSetting::defaultScene = Resources::JsonStrToString(data["DefaultScene"]);
	}
}