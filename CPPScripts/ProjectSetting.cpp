#include "ProjectSetting.h"
#include "Resources.h"

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

		ProjectSetting::srcWidth = data["WindowSize"][0];
		ProjectSetting::srcHeight = data["WindowSize"][1];
		ProjectSetting::defaultScene = Resources::JsonStrToString(data["DefaultScene"]);
	}
}