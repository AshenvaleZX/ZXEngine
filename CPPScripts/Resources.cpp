#include "Resources.h"

namespace ZXEngine
{
	const char* Resources::assetsPath;
	void Resources::SetAssetsPath(const char* path)
	{
		assetsPath = path;
	}

	SceneStruct* Resources::LoadScene(const char* path)
	{
		std::stringstream ss;
		ss << assetsPath << path;
		Debug::Log(ss.str());
		std::ifstream f(ss.str());
		json data = json::parse(f);
		SceneStruct* scene = new SceneStruct;

		for (unsigned int i = 0; i < data["Cameras"].size(); i++)
		{
			Debug::Log(to_string(data["Cameras"][i]));
			CameraStruct* cam = new CameraStruct;
			scene->cameras.push_back(cam);
		}

		for (unsigned int i = 0; i < data["GameObjects"].size(); i++)
		{
			Debug::Log(to_string(data["GameObjects"][i]));
			PrefabStruct* prefab = new PrefabStruct;
			scene->prefabs.push_back(prefab);
		}

		return scene;
	}
}